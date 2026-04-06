#include "GameManager.h"
#include <3ds.h>
#include <iostream>
#include <string>
#include <chrono>
#include "Ground.h"
#include "Player.h"
#include "Constants.h"

GameManager::GameManager(int state) {}

void GameManager::init()
{
    // system init
    gfxInitDefault();
    romfsInit();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    // screen target init
    topRight = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    // console init
    consoleInit(GFX_BOTTOM, NULL);
    // object init
    objects.push_back(std::make_unique<Ground>(
        0, (SCREEN_HEIGHT / 4) * 3, SCREEN_HEIGHT / 4, SCREEN_WIDTH));
    player = Player(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_HEIGHT / 2, 64, 3, 20);
}
void GameManager::exit()
{
    // system exit
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
}
void GameManager::draw()
{
    // scene preparation
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(topRight, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topRight);
    // draw
    player.draw();
    for (auto &obj : objects)
    {
        obj->draw();
    }
    // end
    C3D_FrameEnd(0);
}

void GameManager::update(int &s)
{
    gspWaitForVBlank();
    // gfxSwapBuffers();

    hidScanInput();
    u32 kDown = hidKeysDown();
    u32 kHeld = hidKeysHeld();

    if (kDown & KEY_START)
        s = -1;
    if (kHeld & KEY_LEFT)
        player.moveLeft();
    if (kHeld & KEY_RIGHT)
        player.moveRight();
}

std::vector<std::unique_ptr<BaseObject>> &GameManager::get_objects()
{
    return objects;
}
