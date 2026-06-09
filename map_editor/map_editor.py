import copy
import json
import math
import re
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

APP_TITLE = "3DS Map Editor"
MAP_HEIGHT = 240
MIN_MAP_WIDTH = 400
FPS = 60
CANVAS_PADDING = 40
GRID_COLOR = "#263241"
BACKGROUND_COLOR = "#111820"
MAP_COLOR = "#17212b"
SELECTION_COLOR = "#ffd166"
TEXT_COLOR = "#e8eef5"
TYPE_NAMES = {
    "player": "Player",
    "ground": "Ground",
    "ground_enemy": "Ground Enemy",
    "fly_enemy": "Fly Enemy",
    "powerup": "Powerup",
}
TYPE_COLORS = {
    "player": "#ef476f",
    "ground": "#43aa8b",
    "ground_enemy": "#4d96ff",
    "fly_enemy": "#00c2d1",
    "powerup": "#b86bff",
}
GROUND_MODES = ["static", "horizontal", "vertical", "descent", "rise"]
GROUND_PATROLS = ["fixed", "wall_to_wall", "radius"]
FLY_PATROLS = ["fixed", "horizontal", "vertical"]
ATTACK_TYPES = ["shot", "sword"]
DEFAULT_PRESETS = {
    "player": {"width": 30, "height": 60},
    "ground": {"width": 120, "height": 20, "spped": 1},
    "ground_enemy": {"width": 30, "height": 60},
    "fly_enemy": {"width": 30, "height": 30},
}
POWERUP_SIZE = 30
MIN_GROUND_WIDTH = 20


def project_root():
    return Path(__file__).resolve().parent.parent


def maps_folder():
    return project_root() / "maps"


def settings_path():
    return Path(__file__).resolve().parent / "settings.json"


def clean_number(value):
    value = float(value)
    if value.is_integer():
        return int(value)
    return round(value, 4)


def ascii_title(value):
    return "".join(char for char in value if 32 <= ord(char) <= 126)


def file_title(value):
    value = ascii_title(value).strip()
    value = re.sub(r'[<>:"/\\|?*]', "_", value)
    value = value.rstrip(". ")
    return value or "untitled"


def new_uid(counter):
    return f"object_{counter}"


class MapModel:
    def __init__(self, presets):
        self.presets = presets
        self.uid_counter = 1
        self.data = {}
        self.new_map()

    def next_uid(self):
        uid = new_uid(self.uid_counter)
        self.uid_counter += 1
        return uid

    def new_map(self):
        self.uid_counter = 1
        self.data = {
            "title": "NEW_MAP",
            "player": {
                "_id": self.next_uid(),
                "_type": "player",
                "x": 40.0,
                "y": 160.0,
            },
            "grounds": [],
            "ground_enemies": [],
            "fly_enemies": [],
            "powerups": [],
        }
        self.add_object(
            "ground", 0, MAP_HEIGHT - 20, width=MIN_MAP_WIDTH, height=20
        )

    def load(self, path):
        with open(path, "r", encoding="utf-8") as file:
            source = json.load(file)

        params = source["map_params"]
        player = source["player"]
        self.uid_counter = 1
        self.data = {
            "title": ascii_title(str(params["title"])),
            "player": {
                "_id": self.next_uid(),
                "_type": "player",
                "x": float(player["x"]),
                "y": float(player["y"]),
            },
            "grounds": [],
            "ground_enemies": [],
            "fly_enemies": [],
            "powerups": [],
        }

        for source_object in source["grounds"]:
            obj = copy.deepcopy(source_object)
            obj["_id"] = self.next_uid()
            obj["_type"] = "ground"
            self.data["grounds"].append(obj)

        for source_object in source["ground_enemies"]:
            obj = copy.deepcopy(source_object)
            obj["_id"] = self.next_uid()
            obj["_type"] = "ground_enemy"
            self.data["ground_enemies"].append(obj)

        for source_object in source["fly_enemies"]:
            obj = copy.deepcopy(source_object)
            obj["_id"] = self.next_uid()
            obj["_type"] = "fly_enemy"
            self.data["fly_enemies"].append(obj)

        for source_object in source["powerups"]:
            obj = copy.deepcopy(source_object)
            obj["_id"] = self.next_uid()
            obj["_type"] = "powerup"
            self.data["powerups"].append(obj)

        self.normalize()

    def all_objects(self):
        objects = [self.data["player"]]
        objects.extend(self.data["grounds"])
        objects.extend(self.data["ground_enemies"])
        objects.extend(self.data["fly_enemies"])
        objects.extend(self.data["powerups"])
        return objects

    def find(self, uid):
        for obj in self.all_objects():
            if obj["_id"] == uid:
                return obj
        return None

    def object_size(self, obj):
        if obj["_type"] == "player":
            preset = self.presets["player"]
            return float(preset["width"]), float(preset["height"])
        if obj["_type"] == "powerup":
            return POWERUP_SIZE, POWERUP_SIZE
        return float(obj["width"]), float(obj["height"])

    def map_width(self):
        right = MIN_MAP_WIDTH
        for obj in self.all_objects():
            width, _ = self.object_size(obj)
            right = max(right, float(obj["x"]) + width)
            if (
                obj["_type"] == "ground"
                and obj.get("mode") in GROUND_MODES[1:]
            ):
                radius = float(obj.get("radius", 0))
                if obj["mode"] in ("horizontal", "descent", "rise"):
                    right = max(right, float(obj["x"]) + width + radius)
            if obj["_type"] in ("ground_enemy", "fly_enemy"):
                right = max(
                    right,
                    float(obj["x"])
                    + width
                    + float(obj.get("patrol_radius", 0)),
                )
        return max(MIN_MAP_WIDTH, int(math.ceil(right)))

    def normalize(self):
        for obj in self.all_objects():
            width, height = self.object_size(obj)
            obj["x"] = max(0.0, float(obj["x"]))
            obj["y"] = min(
                max(0.0, float(obj["y"])), max(0.0, MAP_HEIGHT - height)
            )
            if obj["_type"] not in ("player", "powerup"):
                min_width = (
                    MIN_GROUND_WIDTH if obj["_type"] == "ground" else 1.0
                )
                obj["width"] = max(min_width, float(obj["width"]))
                obj["height"] = min(MAP_HEIGHT, max(1.0, float(obj["height"])))

    def add_object(self, object_type, x, y, width=None, height=None):
        if object_type == "powerup":
            width = POWERUP_SIZE
            height = POWERUP_SIZE
        else:
            preset = self.presets[object_type]
            width = float(width if width is not None else preset["width"])
            height = float(height if height is not None else preset["height"])
        common = {
            "_id": self.next_uid(),
            "_type": object_type,
            "x": float(x),
            "y": float(y),
            "height": height,
            "width": width,
        }

        if object_type == "ground":
            common.update(
                {
                    "is_barrier": False,
                    "mode": "static",
                    "radius": 0.0,
                    "speed": 1.0,
                }
            )
            self.data["grounds"].append(common)
        elif object_type == "ground_enemy":
            common.update(
                {
                    "hp": 3,
                    "speed": 1.0,
                    "cooldown": 120,
                    "attack_type": "shot",
                    "aggr_radius": 200.0,
                    "attack_radius": 100.0,
                    "patrol_type": "radius",
                    "patrol_radius": 60.0,
                }
            )
            self.data["ground_enemies"].append(common)
        elif object_type == "fly_enemy":
            common.update(
                {
                    "hp": 3,
                    "speed": 1.0,
                    "cooldown": 120,
                    "attack_type": "shot",
                    "aggr_radius": 220.0,
                    "attack_radius": 120.0,
                    "patrol_type": "horizontal",
                    "patrol_radius": 70.0,
                }
            )
            self.data["fly_enemies"].append(common)
        elif object_type == "powerup":
            common.update(
                {
                    "attack_type": "shot",
                    "duration": 300,
                }
            )
            self.data["powerups"].append(common)
        else:
            return self.data["player"]

        self.normalize()
        return common

    def delete(self, uid):
        if self.data["player"]["_id"] == uid:
            return False
        for name in ("grounds", "ground_enemies", "fly_enemies", "powerups"):
            for index, obj in enumerate(self.data[name]):
                if obj["_id"] == uid:
                    del self.data[name][index]
                    return True
        return False

    def duplicate(self, uid):
        source = self.find(uid)
        if source is None or source["_type"] == "player":
            return None
        obj = copy.deepcopy(source)
        obj["_id"] = self.next_uid()
        obj["x"] = float(obj["x"]) + 10
        obj["y"] = float(obj["y"]) + 10
        collection = {
            "ground": "grounds",
            "ground_enemy": "ground_enemies",
            "fly_enemy": "fly_enemies",
            "powerup": "powerups",
        }[obj["_type"]]
        self.data[collection].append(obj)
        self.normalize()
        return obj

    def export_data(self):
        self.normalize()
        result = {
            "map_params": {
                "width": self.map_width(),
                "title": ascii_title(self.data["title"]),
            },
            "player": {
                "x": clean_number(self.data["player"]["x"]),
                "y": clean_number(self.data["player"]["y"]),
            },
            "grounds": [],
            "ground_enemies": [],
            "fly_enemies": [],
            "powerups": [],
        }

        for obj in self.data["grounds"]:
            result["grounds"].append(
                {
                    "x": clean_number(obj["x"]),
                    "y": clean_number(obj["y"]),
                    "height": clean_number(obj["height"]),
                    "width": clean_number(obj["width"]),
                    "is_barrier": bool(obj["is_barrier"]),
                    "mode": obj["mode"],
                    "radius": clean_number(obj["radius"]),
                    "speed": clean_number(obj["speed"]),
                }
            )

        for name in ("ground_enemies", "fly_enemies"):
            for obj in self.data[name]:
                result[name].append(
                    {
                        "x": clean_number(obj["x"]),
                        "y": clean_number(obj["y"]),
                        "height": clean_number(obj["height"]),
                        "width": clean_number(obj["width"]),
                        "hp": int(obj["hp"]),
                        "speed": clean_number(obj["speed"]),
                        "cooldown": int(round(float(obj["cooldown"]))),
                        "attack_type": obj["attack_type"],
                        "aggr_radius": clean_number(obj["aggr_radius"]),
                        "attack_radius": clean_number(obj["attack_radius"]),
                        "patrol_type": obj["patrol_type"],
                        "patrol_radius": clean_number(obj["patrol_radius"]),
                    }
                )

        for obj in self.data["powerups"]:
            result["powerups"].append(
                {
                    "x": clean_number(obj["x"]),
                    "y": clean_number(obj["y"]),
                    "attack_type": obj["attack_type"],
                    "duration": int(round(float(obj["duration"]))),
                }
            )

        return result


class History:
    def __init__(self, limit=100):
        self.limit = limit
        self.undo_stack = []
        self.redo_stack = []

    def clear(self):
        self.undo_stack.clear()
        self.redo_stack.clear()

    def push(self, data):
        snapshot = copy.deepcopy(data)
        if self.undo_stack and self.undo_stack[-1] == snapshot:
            return
        self.undo_stack.append(snapshot)
        if len(self.undo_stack) > self.limit:
            self.undo_stack.pop(0)
        self.redo_stack.clear()

    def undo(self, current):
        if not self.undo_stack:
            return None
        self.redo_stack.append(copy.deepcopy(current))
        return self.undo_stack.pop()

    def redo(self, current):
        if not self.redo_stack:
            return None
        self.undo_stack.append(copy.deepcopy(current))
        return self.redo_stack.pop()


class PresetDialog(tk.Toplevel):
    def __init__(self, master, presets):
        super().__init__(master)
        self.title("Size Presets")
        self.resizable(False, False)
        self.transient(master)
        self.grab_set()
        self.result = None
        self.variables = {}
        self.configure(padx=18, pady=18)

        ttk.Label(
            self, text="Default object sizes", style="DialogTitle.TLabel"
        ).grid(row=0, column=0, columnspan=3, sticky="w", pady=(0, 14))
        ttk.Label(self, text="Object").grid(
            row=1, column=0, sticky="w", padx=(0, 20)
        )
        ttk.Label(self, text="Width").grid(row=1, column=1, sticky="w")
        ttk.Label(self, text="Height").grid(row=1, column=2, sticky="w")

        preset_types = [
            "player",
            "ground",
            "ground_enemy",
            "fly_enemy",
        ]
        for row, object_type in enumerate(preset_types, start=2):
            width_var = tk.StringVar(value=str(presets[object_type]["width"]))
            height_var = tk.StringVar(
                value=str(presets[object_type]["height"])
            )
            self.variables[object_type] = (width_var, height_var)
            ttk.Label(self, text=TYPE_NAMES[object_type]).grid(
                row=row, column=0, sticky="w", padx=(0, 20), pady=4
            )
            ttk.Entry(self, textvariable=width_var, width=9).grid(
                row=row, column=1, pady=4, padx=(0, 8)
            )
            ttk.Entry(self, textvariable=height_var, width=9).grid(
                row=row, column=2, pady=4
            )

        buttons = ttk.Frame(self)
        buttons.grid(row=8, column=0, columnspan=3, sticky="e", pady=(16, 0))
        ttk.Button(buttons, text="Cancel", command=self.destroy).pack(
            side="left", padx=(0, 8)
        )
        ttk.Button(
            buttons, text="Save", style="Accent.TButton", command=self.save
        ).pack(side="left")
        self.bind("<Escape>", lambda event: self.destroy())
        self.bind("<Return>", lambda event: self.save())

    def save(self):
        result = {}
        try:
            for object_type, variables in self.variables.items():
                width = float(variables[0].get())
                height = float(variables[1].get())
                if width <= 0 or height <= 0:
                    raise ValueError
                if object_type == "ground":
                    width = max(MIN_GROUND_WIDTH, width)
                result[object_type] = {
                    "width": clean_number(width),
                    "height": clean_number(height),
                }
        except ValueError:
            messagebox.showerror(
                "Invalid presets",
                "Width and height must be positive numbers.",
                parent=self,
            )
            return
        self.result = result
        self.destroy()


class MapEditor(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title(APP_TITLE)
        self.geometry("1440x860")
        self.minsize(1100, 700)
        self.protocol("WM_DELETE_WINDOW", self.close)

        self.presets = self.load_settings()
        self.model = MapModel(self.presets)
        self.history = History()
        self.current_path = None
        self.saved_snapshot = copy.deepcopy(self.model.data)
        self.selected_uid = self.model.data["player"]["_id"]
        self.zoom = tk.DoubleVar(value=1.0)
        self.grid_size = tk.IntVar(value=10)
        self.snap_enabled = tk.BooleanVar(value=True)
        self.grid_enabled = tk.BooleanVar(value=True)
        self.radii_enabled = tk.BooleanVar(value=True)
        self.time_unit = tk.StringVar(value="seconds")
        self.status_text = tk.StringVar(value="Ready")
        self.property_vars = {}
        self.property_widgets = {}
        self.canvas_objects = {}
        self.drag_mode = None
        self.drag_start = None
        self.drag_original = None
        self.drag_history = None
        self.clipboard_object = None
        self.pending_property_snapshot = None
        self.properties_scroll_job = None
        self.properties_resize_job = None
        self.properties_resize_width = 1

        self.setup_style()
        self.build_ui()
        self.bind_shortcuts()
        self.refresh_all()
        self.after_idle(self.show_window)

    def show_window(self):
        self.deiconify()
        self.lift()
        self.focus_force()

    def setup_style(self):
        style = ttk.Style(self)
        style.theme_use("clam")
        self.configure(bg="#0d141b")
        style.configure(
            ".",
            background="#151e27",
            foreground=TEXT_COLOR,
            fieldbackground="#1e2a36",
        )
        style.configure("TFrame", background="#151e27")
        style.configure("Panel.TFrame", background="#151e27")
        style.configure("Toolbar.TFrame", background="#101820")
        style.configure("TLabel", background="#151e27", foreground=TEXT_COLOR)
        style.configure("Muted.TLabel", foreground="#91a4b7")
        style.configure("DialogTitle.TLabel", font=("Segoe UI Semibold", 13))
        style.configure(
            "Section.TLabel",
            font=("Segoe UI Semibold", 11),
            foreground="#8ecae6",
        )
        style.configure(
            "TButton",
            padding=(10, 6),
            background="#263746",
            foreground=TEXT_COLOR,
        )
        style.map(
            "TButton",
            background=[("active", "#344b5f"), ("pressed", "#1d2a35")],
        )
        style.configure(
            "Accent.TButton", background="#2878b5", foreground="white"
        )
        style.map(
            "Accent.TButton",
            background=[("active", "#3290d3"), ("pressed", "#1e6499")],
        )
        style.configure(
            "Danger.TButton", background="#873b4b", foreground="white"
        )
        style.map("Danger.TButton", background=[("active", "#a94a5e")])
        style.configure(
            "Treeview",
            background="#111a23",
            fieldbackground="#111a23",
            foreground=TEXT_COLOR,
            rowheight=26,
        )
        style.configure(
            "Treeview.Heading", background="#22303d", foreground=TEXT_COLOR
        )
        style.map("Treeview", background=[("selected", "#245b83")])
        style.configure("TEntry", padding=5)
        style.configure("TCombobox", padding=4)
        style.configure(
            "TCheckbutton", background="#151e27", foreground=TEXT_COLOR
        )
        style.configure("Vertical.TScrollbar", background="#263746")
        style.configure("Horizontal.TScrollbar", background="#263746")
        style.configure(
            "Status.TLabel",
            background="#101820",
            foreground="#a9bac9",
            padding=(10, 5),
        )

    def build_ui(self):
        self.build_menu()
        self.build_toolbar()

        main = ttk.Panedwindow(self, orient="horizontal")
        main.pack(fill="both", expand=True)

        left = ttk.Frame(main, style="Panel.TFrame", width=250)
        center = ttk.Frame(main, style="Panel.TFrame")
        right = ttk.Frame(main, style="Panel.TFrame", width=310)
        main.add(left, weight=0)
        main.add(center, weight=1)
        main.add(right, weight=0)

        self.build_left_panel(left)
        self.build_canvas(center)
        self.build_properties(right)

        ttk.Label(
            self,
            textvariable=self.status_text,
            style="Status.TLabel",
            anchor="w",
        ).pack(fill="x", side="bottom")

    def build_menu(self):
        menu = tk.Menu(self)
        file_menu = tk.Menu(menu, tearoff=False)
        file_menu.add_command(
            label="New", accelerator="Ctrl+N", command=self.new_map
        )
        file_menu.add_command(
            label="Open...", accelerator="Ctrl+O", command=self.open_map
        )
        file_menu.add_separator()
        file_menu.add_command(
            label="Save", accelerator="Ctrl+S", command=self.save_map
        )
        file_menu.add_command(
            label="Save As...",
            accelerator="Ctrl+Shift+S",
            command=self.save_map_as,
        )
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.close)
        menu.add_cascade(label="File", menu=file_menu)

        edit_menu = tk.Menu(menu, tearoff=False)
        edit_menu.add_command(
            label="Undo", accelerator="Ctrl+Z", command=self.undo
        )
        edit_menu.add_command(
            label="Redo", accelerator="Ctrl+Y", command=self.redo
        )
        edit_menu.add_separator()
        edit_menu.add_command(
            label="Copy", accelerator="Ctrl+C", command=self.copy_selected
        )
        edit_menu.add_command(
            label="Paste", accelerator="Ctrl+V", command=self.paste_object
        )
        edit_menu.add_command(
            label="Duplicate",
            accelerator="Ctrl+D",
            command=self.duplicate_selected,
        )
        edit_menu.add_command(
            label="Delete", accelerator="Delete", command=self.delete_selected
        )
        menu.add_cascade(label="Edit", menu=edit_menu)

        view_menu = tk.Menu(menu, tearoff=False)
        view_menu.add_checkbutton(
            label="Grid",
            variable=self.grid_enabled,
            command=self.redraw_canvas,
        )
        view_menu.add_checkbutton(
            label="Snap to grid", variable=self.snap_enabled
        )
        view_menu.add_checkbutton(
            label="Radii",
            variable=self.radii_enabled,
            command=self.redraw_canvas,
        )
        menu.add_cascade(label="View", menu=view_menu)

        settings_menu = tk.Menu(menu, tearoff=False)
        settings_menu.add_command(
            label="Size presets...", command=self.edit_presets
        )
        menu.add_cascade(label="Settings", menu=settings_menu)
        self.config(menu=menu)

    def build_toolbar(self):
        toolbar = ttk.Frame(self, style="Toolbar.TFrame", padding=(8, 7))
        toolbar.pack(fill="x")
        ttk.Button(toolbar, text="New", command=self.new_map).pack(
            side="left", padx=2
        )
        ttk.Button(toolbar, text="Open", command=self.open_map).pack(
            side="left", padx=2
        )
        ttk.Button(
            toolbar, text="Save", style="Accent.TButton", command=self.save_map
        ).pack(side="left", padx=2)
        ttk.Separator(toolbar, orient="vertical").pack(
            side="left", fill="y", padx=8
        )
        ttk.Button(toolbar, text="Undo", command=self.undo).pack(
            side="left", padx=2
        )
        ttk.Button(toolbar, text="Redo", command=self.redo).pack(
            side="left", padx=2
        )
        ttk.Separator(toolbar, orient="vertical").pack(
            side="left", fill="y", padx=8
        )

        ttk.Checkbutton(
            toolbar,
            text="Grid",
            variable=self.grid_enabled,
            command=self.redraw_canvas,
        ).pack(side="left", padx=4)
        ttk.Checkbutton(toolbar, text="Snap", variable=self.snap_enabled).pack(
            side="left", padx=4
        )
        ttk.Checkbutton(
            toolbar,
            text="Radii",
            variable=self.radii_enabled,
            command=self.redraw_canvas,
        ).pack(side="left", padx=4)

        ttk.Label(toolbar, text="Grid:").pack(side="left", padx=(12, 4))
        grid_box = ttk.Combobox(
            toolbar,
            textvariable=self.grid_size,
            values=[1, 5, 10, 20, 25, 40],
            width=5,
        )
        grid_box.pack(side="left")
        grid_box.bind(
            "<<ComboboxSelected>>", lambda event: self.redraw_canvas()
        )

        ttk.Label(toolbar, text="Zoom:").pack(side="left", padx=(12, 4))
        self.zoom_box = ttk.Combobox(
            toolbar,
            values=["50%", "75%", "100%", "125%", "150%", "200%", "300%"],
            width=7,
            state="readonly",
        )
        self.zoom_box.set("100%")
        self.zoom_box.pack(side="left")
        self.zoom_box.bind("<<ComboboxSelected>>", self.change_zoom)

        ttk.Label(toolbar, text="Time:").pack(side="left", padx=(12, 4))
        time_box = ttk.Combobox(
            toolbar,
            textvariable=self.time_unit,
            values=["seconds", "frames"],
            width=8,
            state="readonly",
        )
        time_box.pack(side="left")
        time_box.bind(
            "<<ComboboxSelected>>", lambda event: self.refresh_properties()
        )

        ttk.Button(toolbar, text="Presets", command=self.edit_presets).pack(
            side="right", padx=2
        )

    def build_left_panel(self, parent):
        header = ttk.Frame(parent, padding=10)
        header.pack(fill="x")
        ttk.Label(header, text="Objects", style="Section.TLabel").pack(
            side="left"
        )
        ttk.Button(
            header, text="Duplicate", command=self.duplicate_selected
        ).pack(side="right")

        palette = ttk.Frame(parent, padding=(10, 0, 10, 8))
        palette.pack(fill="x")
        buttons = [
            ("Ground", "ground"),
            ("Ground enemy", "ground_enemy"),
            ("Fly enemy", "fly_enemy"),
            ("Powerup", "powerup"),
        ]
        for index, item in enumerate(buttons):
            button = ttk.Button(
                palette,
                text=f"+ {item[0]}",
                command=lambda value=item[1]: self.add_object(value),
            )
            button.grid(
                row=index // 2, column=index % 2, sticky="ew", padx=3, pady=3
            )
        palette.columnconfigure(0, weight=1)
        palette.columnconfigure(1, weight=1)

        tree_frame = ttk.Frame(parent, padding=(10, 0, 10, 8))
        tree_frame.pack(fill="both", expand=True)
        self.object_tree = ttk.Treeview(
            tree_frame,
            columns=("x", "y"),
            show="tree headings",
            selectmode="browse",
        )
        self.object_tree.heading("#0", text="Type")
        self.object_tree.heading("x", text="X")
        self.object_tree.heading("y", text="Y")
        self.object_tree.column("#0", width=135, stretch=True)
        self.object_tree.column("x", width=48, anchor="e")
        self.object_tree.column("y", width=48, anchor="e")
        tree_scroll = ttk.Scrollbar(
            tree_frame, orient="vertical", command=self.object_tree.yview
        )
        self.object_tree.configure(yscrollcommand=tree_scroll.set)
        self.object_tree.pack(side="left", fill="both", expand=True)
        tree_scroll.pack(side="right", fill="y")
        self.object_tree.bind("<<TreeviewSelect>>", self.tree_select)

        bottom = ttk.Frame(parent, padding=10)
        bottom.pack(fill="x")
        ttk.Button(
            bottom,
            text="Delete",
            style="Danger.TButton",
            command=self.delete_selected,
        ).pack(side="left", fill="x", expand=True)

    def build_canvas(self, parent):
        info = ttk.Frame(parent, padding=(10, 8))
        info.pack(fill="x")
        self.map_info = ttk.Label(info, text="", style="Muted.TLabel")
        self.map_info.pack(side="left")
        ttk.Label(
            info,
            text="Drag to move. Drag the square handle to resize.",
            style="Muted.TLabel",
        ).pack(side="right")

        canvas_frame = ttk.Frame(parent)
        canvas_frame.pack(fill="both", expand=True, padx=8, pady=(0, 8))
        self.canvas = tk.Canvas(
            canvas_frame,
            bg=BACKGROUND_COLOR,
            highlightthickness=0,
            xscrollincrement=1,
            yscrollincrement=1,
        )
        horizontal = ttk.Scrollbar(
            canvas_frame, orient="horizontal", command=self.canvas.xview
        )
        vertical = ttk.Scrollbar(
            canvas_frame, orient="vertical", command=self.canvas.yview
        )
        self.canvas.configure(
            xscrollcommand=horizontal.set, yscrollcommand=vertical.set
        )
        self.canvas.grid(row=0, column=0, sticky="nsew")
        vertical.grid(row=0, column=1, sticky="ns")
        horizontal.grid(row=1, column=0, sticky="ew")
        canvas_frame.rowconfigure(0, weight=1)
        canvas_frame.columnconfigure(0, weight=1)

        self.canvas.bind("<ButtonPress-1>", self.canvas_press)
        self.canvas.bind("<B1-Motion>", self.canvas_drag)
        self.canvas.bind("<ButtonRelease-1>", self.canvas_release)
        self.canvas.bind("<Motion>", self.canvas_motion)
        self.canvas.bind("<MouseWheel>", self.canvas_wheel)
        self.canvas.bind("<Control-MouseWheel>", self.canvas_zoom_wheel)
        self.canvas.bind(
            "<Button-4>", lambda event: self.canvas.yview_scroll(-1, "units")
        )
        self.canvas.bind(
            "<Button-5>", lambda event: self.canvas.yview_scroll(1, "units")
        )
        self.canvas.bind("<Button-3>", self.canvas_context_menu)

    def build_properties(self, parent):
        top = ttk.Frame(parent, padding=10)
        top.pack(fill="x")
        ttk.Label(top, text="Properties", style="Section.TLabel").pack(
            side="left"
        )
        ttk.Button(
            top,
            text="Apply",
            style="Accent.TButton",
            command=self.apply_properties,
        ).pack(side="right")

        map_frame = ttk.LabelFrame(parent, text="Map", padding=10)
        map_frame.pack(fill="x", padx=10, pady=(0, 8))
        ttk.Label(map_frame, text="Title").grid(row=0, column=0, sticky="w")
        self.title_var = tk.StringVar()
        title_entry = ttk.Entry(map_frame, textvariable=self.title_var)
        title_entry.grid(row=1, column=0, sticky="ew", pady=(3, 8))
        title_entry.bind("<FocusIn>", self.property_focus_in)
        title_entry.bind("<FocusOut>", self.apply_map_title)
        title_entry.bind("<Return>", self.apply_map_title)
        self.width_label = ttk.Label(map_frame, text="")
        self.width_label.grid(row=2, column=0, sticky="w")
        ttk.Label(
            map_frame, text=f"Height: {MAP_HEIGHT} px", style="Muted.TLabel"
        ).grid(row=3, column=0, sticky="w", pady=(2, 0))
        map_frame.columnconfigure(0, weight=1)

        properties_outer = ttk.Frame(parent)
        properties_outer.pack(fill="both", expand=True, padx=10, pady=(0, 10))
        self.properties_canvas = tk.Canvas(
            properties_outer,
            bg="#151e27",
            highlightthickness=0,
            width=290,
        )
        properties_scroll = ttk.Scrollbar(
            properties_outer,
            orient="vertical",
            command=self.properties_canvas.yview,
        )
        self.properties_canvas.configure(yscrollcommand=properties_scroll.set)
        properties_scroll.pack(side="right", fill="y")
        self.properties_canvas.pack(side="left", fill="both", expand=True)
        self.properties_frame = ttk.Frame(self.properties_canvas)
        self.properties_window = self.properties_canvas.create_window(
            (0, 0),
            window=self.properties_frame,
            anchor="nw",
        )
        self.properties_frame.bind(
            "<Configure>", self.schedule_properties_scroll
        )
        self.properties_canvas.bind(
            "<Configure>", self.schedule_properties_resize
        )
        self.properties_canvas.bind(
            "<MouseWheel>",
            lambda event: self.properties_canvas.yview_scroll(
                int(-event.delta / 120), "units"
            ),
        )

    def bind_shortcuts(self):
        self.bind_all("<Control-n>", lambda event: self.new_map())
        self.bind_all("<Control-o>", lambda event: self.open_map())
        self.bind_all("<Control-s>", lambda event: self.save_map())
        self.bind_all("<Control-Shift-S>", lambda event: self.save_map_as())
        self.bind_all("<Control-z>", lambda event: self.undo())
        self.bind_all("<Control-y>", lambda event: self.redo())
        self.bind_all("<Control-Shift-Z>", lambda event: self.redo())
        self.bind_all("<Control-c>", self.copy_key)
        self.bind_all("<Control-v>", self.paste_key)
        self.bind_all("<Control-d>", lambda event: self.duplicate_selected())
        self.bind_all("<Delete>", self.delete_key)
        self.bind_all(
            "<Left>", lambda event: self.nudge_selected(event, -1, 0)
        )
        self.bind_all(
            "<Right>", lambda event: self.nudge_selected(event, 1, 0)
        )
        self.bind_all("<Up>", lambda event: self.nudge_selected(event, 0, -1))
        self.bind_all("<Down>", lambda event: self.nudge_selected(event, 0, 1))
        self.bind_all("<Escape>", lambda event: self.canvas.focus_set())

    def load_settings(self):
        presets = copy.deepcopy(DEFAULT_PRESETS)
        path = settings_path()
        if not path.exists():
            return presets
        try:
            with open(path, "r", encoding="utf-8") as file:
                source = json.load(file)
            for object_type in presets:
                if object_type in source.get("presets", {}):
                    values = source["presets"][object_type]
                    width = float(values["width"])
                    height = float(values["height"])
                    if width > 0 and height > 0:
                        presets[object_type] = {
                            "width": clean_number(width),
                            "height": clean_number(height),
                        }
        except (
            OSError,
            ValueError,
            KeyError,
            TypeError,
            json.JSONDecodeError,
        ):
            return copy.deepcopy(DEFAULT_PRESETS)
        return presets

    def save_settings(self):
        path = settings_path()
        path.parent.mkdir(parents=True, exist_ok=True)
        with open(path, "w", encoding="utf-8") as file:
            json.dump(
                {"presets": self.presets}, file, indent=2, ensure_ascii=True
            )

    def edit_presets(self):
        dialog = PresetDialog(self, self.presets)
        self.wait_window(dialog)
        if dialog.result is None:
            return
        before = copy.deepcopy(self.model.data)
        self.presets.clear()
        self.presets.update(dialog.result)
        self.model.presets = self.presets
        self.model.normalize()
        self.history.push(before)
        self.save_settings()
        self.mark_changed()
        self.refresh_all()

    def snapshot(self):
        return copy.deepcopy(self.model.data)

    def is_changed(self):
        return self.model.data != self.saved_snapshot

    def mark_changed(self):
        self.update_title()

    def update_title(self):
        name = self.model.data["title"] or "Untitled"
        marker = " *" if self.is_changed() else ""
        self.title(f"{APP_TITLE} - {name}{marker}")

    def confirm_discard(self):
        self.apply_properties(show_errors=False)
        if not self.is_changed():
            return True
        answer = messagebox.askyesnocancel(
            "Unsaved changes",
            "Save changes before continuing?",
            parent=self,
        )
        if answer is None:
            return False
        if answer:
            return self.save_map()
        return True

    def new_map(self):
        if not self.confirm_discard():
            return
        self.model.new_map()
        self.history.clear()
        self.current_path = None
        self.saved_snapshot = self.snapshot()
        self.selected_uid = self.model.data["player"]["_id"]
        self.refresh_all()
        self.status_text.set("New map created")

    def open_map(self):
        if not self.confirm_discard():
            return
        path = filedialog.askopenfilename(
            parent=self,
            title="Open map",
            initialdir=maps_folder(),
            filetypes=[("JSON maps", "*.json"), ("All files", "*.*")],
        )
        if not path:
            return
        try:
            self.model.load(path)
        except (
            OSError,
            KeyError,
            ValueError,
            TypeError,
            json.JSONDecodeError,
        ) as error:
            messagebox.showerror(
                "Open failed", f"Could not open the map:\n{error}", parent=self
            )
            return
        self.history.clear()
        self.current_path = Path(path)
        self.saved_snapshot = self.snapshot()
        self.selected_uid = self.model.data["player"]["_id"]
        self.refresh_all()
        self.status_text.set(f"Opened {Path(path).name}")

    def save_map(self):
        if not self.apply_properties():
            return False
        title = ascii_title(self.model.data["title"]).strip()
        if not title:
            messagebox.showerror(
                "Invalid title", "Map title cannot be empty.", parent=self
            )
            return False
        directory = (
            self.current_path.parent if self.current_path else maps_folder()
        )
        path = directory / f"{file_title(title)}.json"
        return self.write_map(path)

    def save_map_as(self):
        if not self.apply_properties():
            return False
        title = ascii_title(self.model.data["title"]).strip()
        if not title:
            messagebox.showerror(
                "Invalid title", "Map title cannot be empty.", parent=self
            )
            return False
        directory = filedialog.askdirectory(
            parent=self,
            title="Choose map folder",
            initialdir=(
                self.current_path.parent
                if self.current_path
                else maps_folder()
            ),
        )
        if not directory:
            return False
        return self.write_map(Path(directory) / f"{file_title(title)}.json")

    def write_map(self, path):
        if path.exists() and (
            self.current_path is None
            or path.resolve() != self.current_path.resolve()
        ):
            overwrite = messagebox.askyesno(
                "Replace map",
                f"{path.name} already exists. Replace it?",
                parent=self,
            )
            if not overwrite:
                return False
        try:
            path.parent.mkdir(parents=True, exist_ok=True)
            with open(path, "w", encoding="utf-8", newline="\n") as file:
                json.dump(
                    self.model.export_data(), file, indent=2, ensure_ascii=True
                )
                file.write("\n")
        except OSError as error:
            messagebox.showerror(
                "Save failed", f"Could not save the map:\n{error}", parent=self
            )
            return False
        self.current_path = path
        self.saved_snapshot = self.snapshot()
        self.update_title()
        self.status_text.set(f"Saved {path.name}")
        return True

    def close(self):
        if self.confirm_discard():
            self.destroy()

    def push_change(self, before):
        if before != self.model.data:
            self.history.push(before)
            self.mark_changed()

    def undo(self):
        data = self.history.undo(self.model.data)
        if data is None:
            self.status_text.set("Nothing to undo")
            return
        self.model.data = data
        self.ensure_selection()
        self.refresh_all()
        self.status_text.set("Undo")

    def redo(self):
        data = self.history.redo(self.model.data)
        if data is None:
            self.status_text.set("Nothing to redo")
            return
        self.model.data = data
        self.ensure_selection()
        self.refresh_all()
        self.status_text.set("Redo")

    def ensure_selection(self):
        if self.model.find(self.selected_uid) is None:
            self.selected_uid = self.model.data["player"]["_id"]

    def snap(self, value):
        if not self.snap_enabled.get():
            return value
        size = max(1, int(self.grid_size.get()))
        return round(value / size) * size

    def visible_center(self):
        zoom = self.zoom.get()
        left = (self.canvas.canvasx(0) - CANVAS_PADDING) / zoom
        right = (
            self.canvas.canvasx(self.canvas.winfo_width()) - CANVAS_PADDING
        ) / zoom
        top = (self.canvas.canvasy(0) - CANVAS_PADDING) / zoom
        bottom = (
            self.canvas.canvasy(self.canvas.winfo_height()) - CANVAS_PADDING
        ) / zoom
        return max(0, (left + right) / 2), min(
            MAP_HEIGHT, max(0, (top + bottom) / 2)
        )

    def add_object(self, object_type):
        before = self.snapshot()
        x, y = self.visible_center()
        if object_type == "powerup":
            width = POWERUP_SIZE
            height = POWERUP_SIZE
        else:
            width = float(self.presets[object_type]["width"])
            height = float(self.presets[object_type]["height"])
        x = self.snap(max(0, x - width / 2))
        y = self.snap(min(max(0, y - height / 2), MAP_HEIGHT - height))
        obj = self.model.add_object(object_type, x, y)
        self.selected_uid = obj["_id"]
        self.push_change(before)
        self.refresh_all()
        self.status_text.set(f"Added {TYPE_NAMES[object_type]}")

    def duplicate_selected(self):
        before = self.snapshot()
        obj = self.model.duplicate(self.selected_uid)
        if obj is None:
            self.status_text.set("Player cannot be duplicated")
            return
        self.selected_uid = obj["_id"]
        self.push_change(before)
        self.refresh_all()
        self.status_text.set("Object duplicated")

    def copy_selected(self):
        obj = self.model.find(self.selected_uid)
        if obj is None or obj["_type"] == "player":
            self.status_text.set("Player cannot be copied")
            return
        self.clipboard_object = copy.deepcopy(obj)
        self.status_text.set(f"Copied {TYPE_NAMES[obj['_type']]}")

    def paste_object(self):
        if self.clipboard_object is None:
            self.status_text.set("Clipboard is empty")
            return
        before = self.snapshot()
        source = self.clipboard_object
        width, height = self.model.object_size(source)
        obj = self.model.add_object(
            source["_type"],
            float(source["x"]) + 10,
            float(source["y"]) + 10,
            width,
            height,
        )
        for key, value in source.items():
            if key not in ("_id", "x", "y"):
                obj[key] = copy.deepcopy(value)
        self.model.normalize()
        self.selected_uid = obj["_id"]
        self.push_change(before)
        self.refresh_all()
        self.status_text.set("Object pasted")

    def copy_key(self, event):
        focused = self.focus_get()
        if isinstance(focused, (tk.Entry, ttk.Entry, ttk.Combobox, tk.Text)):
            return
        self.copy_selected()
        return "break"

    def paste_key(self, event):
        focused = self.focus_get()
        if isinstance(focused, (tk.Entry, ttk.Entry, ttk.Combobox, tk.Text)):
            return
        self.paste_object()
        return "break"

    def nudge_selected(self, event, dx, dy):
        focused = self.focus_get()
        if focused != self.canvas:
            return
        obj = self.model.find(self.selected_uid)
        if obj is None:
            return
        before = self.snapshot()
        step = (
            max(1, int(self.grid_size.get())) if self.snap_enabled.get() else 1
        )
        if event.state & 0x0001:
            step *= 5
        _, height = self.model.object_size(obj)
        obj["x"] = max(0, float(obj["x"]) + dx * step)
        obj["y"] = min(
            max(0, float(obj["y"]) + dy * step),
            MAP_HEIGHT - height,
        )
        self.push_change(before)
        self.refresh_all()
        return "break"

    def delete_selected(self):
        before = self.snapshot()
        if not self.model.delete(self.selected_uid):
            self.status_text.set("Player cannot be deleted")
            return
        self.selected_uid = self.model.data["player"]["_id"]
        self.push_change(before)
        self.refresh_all()
        self.status_text.set("Object deleted")

    def delete_key(self, event):
        focused = self.focus_get()
        if isinstance(focused, (tk.Entry, ttk.Entry, ttk.Combobox, tk.Text)):
            return
        self.delete_selected()

    def change_zoom(self, event=None):
        value = self.zoom_box.get().replace("%", "")
        try:
            zoom = float(value) / 100
        except ValueError:
            return
        self.set_zoom(zoom)

    def set_zoom(self, zoom, focus_x=None, focus_y=None):
        old_zoom = self.zoom.get()
        zoom = min(3.0, max(0.5, zoom))
        if abs(old_zoom - zoom) < 0.001:
            return
        if focus_x is None:
            focus_x = self.canvas.winfo_width() / 2
        if focus_y is None:
            focus_y = self.canvas.winfo_height() / 2
        world_x = (self.canvas.canvasx(focus_x) - CANVAS_PADDING) / old_zoom
        world_y = (self.canvas.canvasy(focus_y) - CANVAS_PADDING) / old_zoom
        self.zoom.set(zoom)
        self.zoom_box.set(f"{int(round(zoom * 100))}%")
        self.redraw_canvas()
        total_width = max(1, self.model.map_width() * zoom + 80)
        total_height = max(1, MAP_HEIGHT * zoom + 80)
        target_x = CANVAS_PADDING + world_x * zoom - focus_x
        target_y = CANVAS_PADDING + world_y * zoom - focus_y
        self.canvas.xview_moveto(max(0, target_x / total_width))
        self.canvas.yview_moveto(max(0, target_y / total_height))

    def canvas_zoom_wheel(self, event):
        direction = 1 if event.delta > 0 else -1
        self.set_zoom(self.zoom.get() + direction * 0.25, event.x, event.y)
        return "break"

    def canvas_wheel(self, event):
        if event.state & 0x0001:
            self.canvas.xview_scroll(int(-event.delta / 120), "units")
        else:
            self.canvas.yview_scroll(int(-event.delta / 120), "units")
        return "break"

    def canvas_context_menu(self, event):
        menu = tk.Menu(self, tearoff=False)
        menu.add_command(label="Duplicate", command=self.duplicate_selected)
        menu.add_command(label="Delete", command=self.delete_selected)
        menu.add_separator()
        for object_type in ("ground", "ground_enemy", "fly_enemy", "powerup"):
            world_x = (
                self.canvas.canvasx(event.x) - CANVAS_PADDING
            ) / self.zoom.get()
            world_y = (
                self.canvas.canvasy(event.y) - CANVAS_PADDING
            ) / self.zoom.get()
            menu.add_command(
                label=f"Add {TYPE_NAMES[object_type]} here",
                command=lambda value=object_type, x=world_x, y=world_y: self.add_object_at(
                    value, x, y
                ),
            )
        menu.tk_popup(event.x_root, event.y_root)

    def add_object_at(self, object_type, x, y):
        before = self.snapshot()
        if object_type == "powerup":
            width = POWERUP_SIZE
            height = POWERUP_SIZE
        else:
            width = float(self.presets[object_type]["width"])
            height = float(self.presets[object_type]["height"])
        x = self.snap(max(0, x - width / 2))
        y = self.snap(min(max(0, y - height / 2), MAP_HEIGHT - height))
        obj = self.model.add_object(object_type, x, y)
        self.selected_uid = obj["_id"]
        self.push_change(before)
        self.refresh_all()

    def canvas_press(self, event):
        self.canvas.focus_set()
        canvas_x = self.canvas.canvasx(event.x)
        canvas_y = self.canvas.canvasy(event.y)
        item = self.find_canvas_item(canvas_x, canvas_y)
        if item is None:
            return
        tags = self.canvas.gettags(item)
        uid = None
        handle = False
        for tag in tags:
            if tag.startswith("uid:"):
                uid = tag[4:]
            if tag == "resize_handle":
                handle = True
        if uid is None:
            return
        self.selected_uid = uid
        obj = self.model.find(uid)
        if obj is None:
            return
        self.drag_mode = (
            "resize"
            if handle and obj["_type"] not in ("player", "powerup")
            else "move"
        )
        self.drag_start = (
            (canvas_x - CANVAS_PADDING) / self.zoom.get(),
            (canvas_y - CANVAS_PADDING) / self.zoom.get(),
        )
        self.drag_original = copy.deepcopy(obj)
        self.drag_history = self.snapshot()
        self.refresh_selection()
        self.refresh_properties()

    def find_canvas_item(self, x, y):
        items = self.canvas.find_overlapping(x - 3, y - 3, x + 3, y + 3)
        for item in reversed(items):
            tags = self.canvas.gettags(item)
            if "resize_handle" in tags:
                return item
        for item in reversed(items):
            if any(
                tag.startswith("uid:") for tag in self.canvas.gettags(item)
            ):
                return item
        return None

    def canvas_drag(self, event):
        if self.drag_mode is None or self.drag_start is None:
            return
        obj = self.model.find(self.selected_uid)
        if obj is None:
            return
        world_x = (
            self.canvas.canvasx(event.x) - CANVAS_PADDING
        ) / self.zoom.get()
        world_y = (
            self.canvas.canvasy(event.y) - CANVAS_PADDING
        ) / self.zoom.get()
        dx = world_x - self.drag_start[0]
        dy = world_y - self.drag_start[1]

        if self.drag_mode == "move":
            width, height = self.model.object_size(obj)
            obj["x"] = self.snap(max(0, float(self.drag_original["x"]) + dx))
            obj["y"] = self.snap(
                min(
                    max(0, float(self.drag_original["y"]) + dy),
                    MAP_HEIGHT - height,
                )
            )
        else:
            width = self.snap(max(1, float(self.drag_original["width"]) + dx))
            height = self.snap(
                max(1, float(self.drag_original["height"]) + dy)
            )
            min_width = (
                MIN_GROUND_WIDTH if obj["_type"] == "ground" else 1
            )
            obj["width"] = max(min_width, width)
            obj["height"] = min(max(1, height), MAP_HEIGHT - float(obj["y"]))

        self.redraw_canvas()
        self.refresh_tree_values()
        self.refresh_properties_values()

    def canvas_release(self, event):
        if self.drag_mode is not None and self.drag_history is not None:
            self.model.normalize()
            self.push_change(self.drag_history)
            self.refresh_all()
        self.drag_mode = None
        self.drag_start = None
        self.drag_original = None
        self.drag_history = None

    def canvas_motion(self, event):
        world_x = (
            self.canvas.canvasx(event.x) - CANVAS_PADDING
        ) / self.zoom.get()
        world_y = (
            self.canvas.canvasy(event.y) - CANVAS_PADDING
        ) / self.zoom.get()
        self.status_text.set(
            f"X: {clean_number(max(0, world_x))}  Y: {clean_number(max(0, min(MAP_HEIGHT, world_y)))}"
        )

    def tree_select(self, event=None):
        selection = self.object_tree.selection()
        if not selection:
            return
        if selection[0] == self.selected_uid:
            return
        self.selected_uid = selection[0]
        self.refresh_properties()
        self.redraw_canvas()

    def refresh_all(self):
        self.model.normalize()
        self.refresh_tree()
        self.refresh_properties()
        self.redraw_canvas()
        self.update_title()

    def refresh_tree(self):
        self.object_tree.delete(*self.object_tree.get_children())
        counters = {key: 0 for key in TYPE_NAMES}
        for obj in self.model.all_objects():
            object_type = obj["_type"]
            counters[object_type] += 1
            label = TYPE_NAMES[object_type]
            if object_type != "player":
                label = f"{label} {counters[object_type]}"
            self.object_tree.insert(
                "",
                "end",
                iid=obj["_id"],
                text=label,
                values=(clean_number(obj["x"]), clean_number(obj["y"])),
            )
        if self.model.find(self.selected_uid):
            self.object_tree.selection_set(self.selected_uid)
            self.object_tree.see(self.selected_uid)

    def refresh_tree_values(self):
        for obj in self.model.all_objects():
            if self.object_tree.exists(obj["_id"]):
                self.object_tree.item(
                    obj["_id"],
                    values=(clean_number(obj["x"]), clean_number(obj["y"])),
                )

    def refresh_selection(self):
        if self.object_tree.exists(self.selected_uid):
            self.object_tree.selection_set(self.selected_uid)

    def redraw_canvas(self):
        self.canvas.delete("all")
        self.canvas_objects.clear()
        zoom = self.zoom.get()
        width = self.model.map_width()
        padding = CANVAS_PADDING
        map_x = padding
        map_y = padding
        map_width = width * zoom
        map_height = MAP_HEIGHT * zoom
        self.canvas.configure(
            scrollregion=(
                0,
                0,
                map_width + padding * 2,
                map_height + padding * 2,
            )
        )
        self.canvas.create_rectangle(
            map_x,
            map_y,
            map_x + map_width,
            map_y + map_height,
            fill=MAP_COLOR,
            outline="#506274",
            width=2,
            tags=("map_background",),
        )

        if self.grid_enabled.get():
            self.draw_grid(map_x, map_y, width, zoom)

        if self.radii_enabled.get():
            for obj in self.model.all_objects():
                self.draw_radii(obj, map_x, map_y, zoom)

        for obj in self.model.all_objects():
            self.draw_object(obj, map_x, map_y, zoom)

        selected = self.model.find(self.selected_uid)
        if selected is not None:
            self.draw_selection(selected, map_x, map_y, zoom)

        self.map_info.configure(
            text=f"{self.model.data['title']}   {width} x {MAP_HEIGHT} px   {len(self.model.all_objects())} objects"
        )
        self.width_label.configure(text=f"Auto width: {width} px")

    def draw_grid(self, map_x, map_y, width, zoom):
        size = max(1, int(self.grid_size.get()))
        if size * zoom < 4:
            size *= math.ceil(4 / (size * zoom))
        for x in range(0, width + 1, size):
            screen_x = map_x + x * zoom
            color = "#344456" if x % 100 == 0 else GRID_COLOR
            self.canvas.create_line(
                screen_x,
                map_y,
                screen_x,
                map_y + MAP_HEIGHT * zoom,
                fill=color,
            )
        for y in range(0, MAP_HEIGHT + 1, size):
            screen_y = map_y + y * zoom
            color = "#344456" if y % 100 == 0 else GRID_COLOR
            self.canvas.create_line(
                map_x,
                screen_y,
                map_x + width * zoom,
                screen_y,
                fill=color,
            )

    def draw_radii(self, obj, map_x, map_y, zoom):
        object_type = obj["_type"]
        width, height = self.model.object_size(obj)
        center_x = map_x + (float(obj["x"]) + width / 2) * zoom
        center_y = map_y + (float(obj["y"]) + height / 2) * zoom

        if object_type == "ground":
            radius = float(obj.get("radius", 0)) * zoom
            mode = obj.get("mode")
            if radius <= 0 or mode == "static":
                return
            if mode == "horizontal":
                self.canvas.create_line(
                    center_x - radius,
                    center_y,
                    center_x + radius,
                    center_y,
                    fill="#74c69d",
                    dash=(5, 4),
                    width=2,
                )
            elif mode == "vertical":
                self.canvas.create_line(
                    center_x,
                    center_y - radius,
                    center_x,
                    center_y + radius,
                    fill="#74c69d",
                    dash=(5, 4),
                    width=2,
                )
            else:
                direction = 1 if mode == "descent" else -1
                component = radius / math.sqrt(2)
                self.canvas.create_line(
                    center_x - component,
                    center_y - component * direction,
                    center_x + component,
                    center_y + component * direction,
                    fill="#74c69d",
                    dash=(5, 4),
                    width=2,
                )

        if object_type in ("ground_enemy", "fly_enemy"):
            attack = float(obj.get("attack_radius", 0)) * zoom
            aggr = float(obj.get("aggr_radius", 0)) * zoom
            patrol = float(obj.get("patrol_radius", 0)) * zoom
            if aggr > 0:
                self.canvas.create_oval(
                    center_x - aggr,
                    center_y - aggr,
                    center_x + aggr,
                    center_y + aggr,
                    outline="#ff9f1c",
                    dash=(4, 4),
                )
            if attack > 0:
                self.canvas.create_oval(
                    center_x - attack,
                    center_y - attack,
                    center_x + attack,
                    center_y + attack,
                    outline="#ef476f",
                    dash=(3, 3),
                )
            if patrol > 0 and obj.get("patrol_type") != "fixed":
                if obj.get("patrol_type") == "vertical":
                    self.canvas.create_line(
                        center_x,
                        center_y - patrol,
                        center_x,
                        center_y + patrol,
                        fill="#8ecae6",
                        dash=(5, 4),
                    )
                else:
                    self.canvas.create_line(
                        center_x - patrol,
                        center_y,
                        center_x + patrol,
                        center_y,
                        fill="#8ecae6",
                        dash=(5, 4),
                    )

    def draw_object(self, obj, map_x, map_y, zoom):
        width, height = self.model.object_size(obj)
        x1 = map_x + float(obj["x"]) * zoom
        y1 = map_y + float(obj["y"]) * zoom
        x2 = x1 + width * zoom
        y2 = y1 + height * zoom
        object_type = obj["_type"]
        color = TYPE_COLORS[object_type]
        tags = (f"uid:{obj['_id']}", "map_object")
        rectangle = self.canvas.create_rectangle(
            x1,
            y1,
            x2,
            y2,
            fill=color,
            outline="#d7e3ec",
            width=1,
            tags=tags,
        )
        self.canvas_objects[obj["_id"]] = rectangle
        if (x2 - x1) >= 36 and (y2 - y1) >= 18:
            label = {
                "player": "P",
                "ground": "G",
                "ground_enemy": "GE",
                "fly_enemy": "FE",
                "powerup": "PU",
            }[object_type]
            self.canvas.create_text(
                (x1 + x2) / 2,
                (y1 + y2) / 2,
                text=label,
                fill="#081018",
                font=("Segoe UI Semibold", max(8, min(12, int(10 * zoom)))),
                tags=tags,
            )

    def draw_selection(self, obj, map_x, map_y, zoom):
        width, height = self.model.object_size(obj)
        x1 = map_x + float(obj["x"]) * zoom
        y1 = map_y + float(obj["y"]) * zoom
        x2 = x1 + width * zoom
        y2 = y1 + height * zoom
        tags = (f"uid:{obj['_id']}", "selection")
        self.canvas.create_rectangle(
            x1 - 2,
            y1 - 2,
            x2 + 2,
            y2 + 2,
            outline=SELECTION_COLOR,
            width=2,
            tags=tags,
        )
        if obj["_type"] not in ("player", "powerup"):
            size = 8
            self.canvas.create_rectangle(
                x2 - size / 2,
                y2 - size / 2,
                x2 + size / 2,
                y2 + size / 2,
                fill=SELECTION_COLOR,
                outline="#5c4300",
                tags=(f"uid:{obj['_id']}", "resize_handle"),
            )

    def refresh_properties(self):
        for child in self.properties_frame.winfo_children():
            child.destroy()
        self.property_vars.clear()
        self.property_widgets.clear()
        obj = self.model.find(self.selected_uid)
        self.title_var.set(self.model.data["title"])
        self.width_label.configure(
            text=f"Auto width: {self.model.map_width()} px"
        )
        if obj is None:
            return

        ttk.Label(
            self.properties_frame,
            text=TYPE_NAMES[obj["_type"]],
            style="Section.TLabel",
        ).grid(row=0, column=0, columnspan=2, sticky="w", pady=(2, 12))

        fields = self.object_fields(obj)
        row = 1
        for field in fields:
            name = field["name"]
            label = field["label"]
            field_type = field.get("type", "number")
            ttk.Label(self.properties_frame, text=label).grid(
                row=row,
                column=0,
                sticky="w",
                padx=(0, 10),
                pady=5,
            )
            if field_type == "bool":
                var = tk.BooleanVar(value=bool(obj[name]))
                widget = ttk.Checkbutton(
                    self.properties_frame,
                    variable=var,
                    command=lambda key=name: self.quick_property_apply(key),
                )
            elif field_type == "choice":
                var = tk.StringVar(value=str(obj[name]))
                widget = ttk.Combobox(
                    self.properties_frame,
                    textvariable=var,
                    values=field["values"],
                    state="readonly",
                )
                widget.bind(
                    "<<ComboboxSelected>>",
                    lambda event, key=name: self.quick_property_apply(key),
                )
            else:
                value = obj[name]
                if (
                    name in ("cooldown", "duration")
                    and self.time_unit.get() == "seconds"
                ):
                    value = float(value) / FPS
                var = tk.StringVar(value=str(clean_number(value)))
                widget = ttk.Entry(self.properties_frame, textvariable=var)
                widget.bind("<FocusIn>", self.property_focus_in)
                widget.bind(
                    "<FocusOut>",
                    lambda event, key=name: self.quick_property_apply(
                        key, False
                    ),
                )
                widget.bind(
                    "<Return>",
                    lambda event, key=name: self.quick_property_apply(key),
                )
            widget.grid(row=row, column=1, sticky="ew", pady=5)
            self.property_vars[name] = var
            self.property_widgets[name] = widget
            row += 1

        self.properties_frame.columnconfigure(1, weight=1)
        self.properties_canvas.yview_moveto(0)

    def object_fields(self, obj):
        fields = [
            {"name": "x", "label": "X"},
            {"name": "y", "label": "Y"},
        ]
        object_type = obj["_type"]
        if object_type not in ("player", "powerup"):
            fields.extend(
                [
                    {"name": "width", "label": "Width"},
                    {"name": "height", "label": "Height"},
                ]
            )
        if object_type == "ground":
            fields.extend(
                [
                    {"name": "is_barrier", "label": "Barrier", "type": "bool"},
                    {
                        "name": "mode",
                        "label": "Movement",
                        "type": "choice",
                        "values": GROUND_MODES,
                    },
                    {"name": "radius", "label": "Move radius"},
                    {"name": "speed", "label": "Speed"},
                ]
            )
        if object_type in ("ground_enemy", "fly_enemy"):
            patrols = (
                GROUND_PATROLS
                if object_type == "ground_enemy"
                else FLY_PATROLS
            )
            fields.extend(
                [
                    {"name": "hp", "label": "HP"},
                    {"name": "speed", "label": "Speed"},
                    {
                        "name": "cooldown",
                        "label": f"Cooldown ({'sec' if self.time_unit.get() == 'seconds' else 'frames'})",
                    },
                    {
                        "name": "attack_type",
                        "label": "Attack",
                        "type": "choice",
                        "values": ATTACK_TYPES,
                    },
                    {"name": "aggr_radius", "label": "Aggro radius"},
                    {"name": "attack_radius", "label": "Attack radius"},
                    {
                        "name": "patrol_type",
                        "label": "Patrol",
                        "type": "choice",
                        "values": patrols,
                    },
                    {"name": "patrol_radius", "label": "Patrol radius"},
                ]
            )
        if object_type == "powerup":
            fields.extend(
                [
                    {
                        "name": "attack_type",
                        "label": "Attack",
                        "type": "choice",
                        "values": ATTACK_TYPES,
                    },
                    {
                        "name": "duration",
                        "label": f"Duration ({'sec' if self.time_unit.get() == 'seconds' else 'frames'})",
                    },
                ]
            )
        return fields

    def property_focus_in(self, event=None):
        self.pending_property_snapshot = self.snapshot()

    def apply_map_title(self, event=None):
        before = self.pending_property_snapshot or self.snapshot()
        value = ascii_title(self.title_var.get())
        self.title_var.set(value)
        self.model.data["title"] = value
        self.pending_property_snapshot = None
        self.push_change(before)
        self.redraw_canvas()
        self.update_title()

    def quick_property_apply(self, field, refresh=True):
        before = self.pending_property_snapshot or self.snapshot()
        if not self.apply_property(field, show_errors=refresh):
            return
        self.pending_property_snapshot = None
        self.push_change(before)
        if refresh:
            self.refresh_all()
        else:
            self.model.normalize()
            self.refresh_tree_values()
            self.redraw_canvas()
            self.update_title()

    def apply_property(self, field, show_errors=True):
        obj = self.model.find(self.selected_uid)
        if obj is None or field not in self.property_vars:
            return False
        var = self.property_vars[field]
        if isinstance(var, tk.BooleanVar):
            obj[field] = bool(var.get())
            return True
        if field in ("mode", "attack_type", "patrol_type"):
            obj[field] = var.get()
            return True
        try:
            value = float(var.get())
            if (
                field in ("cooldown", "duration")
                and self.time_unit.get() == "seconds"
            ):
                value *= FPS
            if field == "hp":
                value = int(round(value))
                if value < 1:
                    raise ValueError
            elif field in ("width", "height"):
                if value <= 0:
                    raise ValueError
                if (
                    field == "width"
                    and obj["_type"] == "ground"
                    and value < MIN_GROUND_WIDTH
                ):
                    value = MIN_GROUND_WIDTH
            elif field not in ("x", "y", "speed"):
                if value < 0:
                    raise ValueError
            obj[field] = value
            self.model.normalize()
            return True
        except ValueError:
            if show_errors:
                messagebox.showerror(
                    "Invalid value", f"Invalid value for {field}.", parent=self
                )
                self.refresh_properties()
            return False

    def apply_properties(self, show_errors=True):
        before = self.pending_property_snapshot or self.snapshot()
        title = ascii_title(self.title_var.get())
        self.model.data["title"] = title
        for field in list(self.property_vars):
            if not self.apply_property(field, show_errors):
                return False
        self.pending_property_snapshot = None
        self.push_change(before)
        self.refresh_all()
        return True

    def refresh_properties_values(self):
        obj = self.model.find(self.selected_uid)
        if obj is None:
            return
        for field, var in self.property_vars.items():
            if isinstance(var, tk.BooleanVar) or field in (
                "mode",
                "attack_type",
                "patrol_type",
            ):
                continue
            value = obj[field]
            if (
                field in ("cooldown", "duration")
                and self.time_unit.get() == "seconds"
            ):
                value = float(value) / FPS
            var.set(str(clean_number(value)))
        self.width_label.configure(
            text=f"Auto width: {self.model.map_width()} px"
        )

    def schedule_properties_scroll(self, event=None):
        if self.properties_scroll_job is not None:
            self.after_cancel(self.properties_scroll_job)
        self.properties_scroll_job = self.after_idle(
            self.update_properties_scroll
        )

    def update_properties_scroll(self):
        self.properties_scroll_job = None
        region = self.properties_canvas.bbox("all")
        if region is None:
            return
        current = self.properties_canvas.cget("scrollregion").split()
        current = tuple(int(float(value)) for value in current)
        if current != region:
            self.properties_canvas.configure(scrollregion=region)

    def schedule_properties_resize(self, event):
        self.properties_resize_width = event.width
        if self.properties_resize_job is not None:
            self.after_cancel(self.properties_resize_job)
        self.properties_resize_job = self.after_idle(
            self.resize_properties_frame
        )

    def resize_properties_frame(self):
        self.properties_resize_job = None
        width = max(1, self.properties_resize_width)
        current = int(
            float(
                self.properties_canvas.itemcget(
                    self.properties_window, "width"
                )
            )
        )
        if current != width:
            self.properties_canvas.itemconfigure(
                self.properties_window, width=width
            )


if __name__ == "__main__":
    app = MapEditor()
    app.mainloop()
