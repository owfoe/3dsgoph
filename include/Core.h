#pragma once
#include <string>
#include <vector>

namespace ProjectSettings
{
    const std::string NAME = "3dsGAME";
    constexpr bool DEBUG = true;
    constexpr bool CONSOLE = false;
    constexpr int FPS = 60;
    constexpr int MENUS_COUNT = 3;
    constexpr int VISIBLE_MAP_COUNT = 6;
    constexpr int GAME_MODE_COUNT = 2;
    constexpr int LOAD_TIME = FPS * 0.5f;
}

namespace PlayerSettings
{
    constexpr int HP = 3;
    constexpr float SPEED = 5.0f;
    constexpr int COOLDOWN = ProjectSettings::FPS * 0.3f;
    constexpr float HEIGHT = 60.0f;
    constexpr float WIDTH = 30.0f;
}

namespace WorldSettings
{
    constexpr float GRAVITY = 0.5f;
    constexpr float MAX_FALL_SPEED = 10.0f;
    constexpr float DEATH_AREA = 100.0f;
}

namespace Const
{
    constexpr float SCREEN_WIDTH = 400.0f;
    constexpr float SCREEN_HEIGHT = 240.0f;
    constexpr float RC_DIFF_H = 3.0f;
    constexpr int TIMER = 60 * 60;

    constexpr float POWERUP_SPEED = 0.4f;
    constexpr float POWERUP_RADIUS = 5.0f;
    constexpr float POWERUP_SIZE = 30.0f;
    constexpr float POWERUP_DIFF_HITBOX = 10.0f;

    constexpr float DEFAULT_X = -1000.0f;
    constexpr float DEFAULT_Y = -1000.0f;

    constexpr float FIX_CONST = 100.0f;

    constexpr float BUBBLE_SIZE = 10.0f;
    constexpr float SHOT_SIZE = 5.0f;

    constexpr float SWORD_RADIUS = 70.0f;

    constexpr float ENEMY_SPAWN_AREA_RADIUS = 5.0f;

    constexpr float CAMERA_LEFT_BORDER = 70.0f;
    constexpr float CAMERA_RIGHT_BORDER = 150.0f;

    constexpr float MARKER_START_X = 34.0f;
    constexpr float MARKER_FORMULA_CONST = 240.0f;

    constexpr float WIN_DX = 10.0f;
}

namespace Path
{
    constexpr const char *CUSTOM_FONT = "romfs:/gfx/Nintendo-NES-Font.bcfnt";
    constexpr const char *HP_SHEET = "romfs:/gfx/hp.t3x";
    constexpr const char *GAME_OVER1_SHEET = "romfs:/gfx/gameover1.t3x";
    constexpr const char *GAME_OVER2_SHEET = "romfs:/gfx/gameover2.t3x";
    constexpr const char *GAME_OVER3_SHEET = "romfs:/gfx/gameover3.t3x";
    constexpr const char *LOGO_SHEET = "romfs:/gfx/logo.t3x";
    constexpr const char *BUBBLE_SHEET = "romfs:/gfx/bubble.t3x";
    constexpr const char *HEAVY_BUBBLE_SHEET = "romfs:/gfx/bigbubble.t3x";
    constexpr const char *NUT_SHEET = "romfs:/gfx/nut.t3x";
    constexpr const char *ENEMY_FLY_SHEET = "romfs:/gfx/enemyfly.t3x";
    constexpr const char *MENU1_SHEET = "romfs:/gfx/menu1.t3x";
    constexpr const char *MENU2_SHEET = "romfs:/gfx/menu2.t3x";
    constexpr const char *MENU3_SHEET = "romfs:/gfx/menu3.t3x";
    constexpr const char *LOWER_MENU_SHEET = "romfs:/gfx/lowermenu.t3x";
    constexpr const char *PWUP_NUT_SHEET = "romfs:/gfx/pwup_nut.t3x";
    constexpr const char *PWUP_SWD_SHEET = "romfs:/gfx/pwup_swd.t3x";
    constexpr const char *INDICATOR_SHEET = "romfs:/gfx/indicator.t3x";
    constexpr const char *PLAYER_MOVE_SHEET = "romfs:/gfx/playermove.t3x";
    constexpr const char *PLAYER_IDLE_SHEET = "romfs:/gfx/playeridle.t3x";
    constexpr const char *PLAYER_JUMP_SHEET = "romfs:/gfx/playerjump.t3x";
    constexpr const char *PLAYER_ATTK_SHEET = "romfs:/gfx/playerattk.t3x";
    constexpr const char *PLAYER_SWD_SHEET = "romfs:/gfx/playerswd.t3x";
    constexpr const char *VICTORY_SHEET = "romfs:/gfx/victory.t3x";
    constexpr const char *PLATFORM_EDGE_SHEET = "romfs:/gfx/platformedge.t3x";
    constexpr const char *PLATFORM_TILE_SHEET = "romfs:/gfx/platformtile.t3x";
    const std::string MARKER = "romfs:/gfx/marker.t3x";
    const std::string LOWER_SCREEN = "romfs:/gfx/lower_screen.t3x";
    const std::string MAPS = "romfs:/maps/";
}

enum class EnemyState
{
    Patrol,
    Aggr,
    Attack
};

enum class EnemyType
{
    Melee,
    Ranged
};

enum class EnemyPatrolType
{
    Fixed,
    WallToWall,
    Radius,
    Horizontal,
    Vertical
};

enum class EntityActionState
{
    Stay,
    Run,
    Attack
};

enum class OwnerType
{
    Player,
    Enemy
};

enum class AttackType
{
    Bubble,
    Shot,
    Sword
};

enum class GroundMode
{
    Static,
    Horizontal,
    Vertical,
    Descent,
    Rise
};

struct PendingAttack
{
    AttackType type;
    bool active = false;
    uint64_t attackTime = 0;
    int view = 1;
    float targetX = 0.0f;
    float targetY = 0.0f;
    float power = 0.0f;
    bool heavyBubble = false;
};

enum class GameManagerState
{
    Title,
    GameModeSelect,
    Game,
    GameOver,
    Victory,
    Load,
    Maps
};

enum class GameMode
{
    Race,
    Clear,
    Testing
};

struct GroundData
{
    float x, y;
    float height, width;
    bool isBarrier;
    GroundMode mode;
    float radius;
    float speed;
};

struct EnemyData
{
    float x, y;
    float height, width;
    int hp;
    float speed;
    uint64_t cooldown;
    AttackType attackType;
    float aggrRadius;
    float attackRadius;
    EnemyPatrolType patrolType;
    float patrolRadius;
};

struct PowerupData
{
    float x, y;
    AttackType attackType;
    uint64_t duration;
};

struct PlayerData
{
    float x, y;
};

struct MapData
{
    std::string title;
    float width;
    PlayerData player;
    std::vector<GroundData> grounds;
    std::vector<EnemyData> groundEnemies;
    std::vector<EnemyData> flyEnemies;
    std::vector<PowerupData> powerups;
};

enum AnimMode
{
    LOOP,
    ONCE
};

struct Animation
{
    C2D_SpriteSheet sheet;
    size_t frameCount;
    int animSpeed;
    AnimMode mode;
};