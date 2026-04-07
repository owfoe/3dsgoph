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
    grounds.push_back(Ground(0, (Const::SCREEN_HEIGHT / 4) * 3, 30, Const::SCREEN_WIDTH / 2));
    player = Player(Const::SCREEN_WIDTH / 4, (Const::SCREEN_HEIGHT / 10) * 0, 30, 30);
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

    // INFO: it can be deleted
    C2D_DrawRectSolid(player.getX(), player.getY() + player.getHeight() - Const::RC_diff_h / 2, 0, player.getWidth(), Const::RC_diff_h, C2D_Color32f(0, 0, 1, 1));

    for (Ground &ground : grounds)
    {
        ground.draw();
    }
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
    if (kDown & KEY_A)
        player.jump();
    player.update(kHeld);

    checkCollisions();
}

void GameManager::checkCollisions()
{
    int i = 0;
    player.setOnGround(false);
    player.setIsJump(true);
    for (Ground &ground : grounds)
    {
        if (Collisions(player.raycast.hitbox, player.getX(), player.getY() + player.getHeight(),
                       ground.hitbox, ground.getX(), ground.getY()))
        {
            player.updateFallOnGround(ground);
            std::cout << "col" << i << std::endl;
            i++;
        }
    }
}
