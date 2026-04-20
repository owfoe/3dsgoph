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
    grounds.push_back(Ground(Const::SCREEN_WIDTH / 3, Const::SCREEN_HEIGHT / 4, 30, 10, false));
    grounds.push_back(Ground(Const::SCREEN_WIDTH / 2, Const::SCREEN_HEIGHT / 4, 30, Const::SCREEN_WIDTH / 8, true, 1, 1, 40));
    grounds.push_back(Ground(Const::SCREEN_WIDTH * 3 / 4, Const::SCREEN_HEIGHT / 4, 30, Const::SCREEN_WIDTH / 8, true, 2, 1, 40));
    grounds.push_back(Ground(0.0f, Const::SCREEN_HEIGHT / 2, 30, Const::SCREEN_WIDTH / 4, false));
    grounds.push_back(Ground(0.0f, (Const::SCREEN_HEIGHT / 4) * 3, 30, Const::SCREEN_WIDTH, false));
    player = Player(0, 0, 60, 30);
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
    // C2D_DrawRectSolid(player.getX(), player.getY() + player.getHeight() / 2 + Const::RC_diff_h, 0, player.getWidth(), player.getHeight() / 2 + Const::RC_diff_h, C2D_Color32f(0, 0, 1, 1));
    player.raycast.hitbox.draw(player.getX(), player.getY());

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
    // gspWaitForVBlank();
    // gfxSwapBuffers();

    hidScanInput();
    u32 kDown = hidKeysDown();
    u32 kHeld = hidKeysHeld();

    if (kDown & KEY_START)
        s = -1;
    // if (kDown & KEY_LEFT)
    //     player.moveLeft();
    // if (kDown & KEY_RIGHT)
    //     player.moveRight();
    if (kHeld & KEY_LEFT)
        player.moveLeft();
    if (kHeld & KEY_RIGHT)
        player.moveRight();
    if (kDown & KEY_A)
        player.jump();
    bool isJumpButtonDown = (kHeld & KEY_A) ? true : false;
    player.update(isJumpButtonDown);
    // std::cout << "Player hitbox: ";
    // player.hitbox.printBorders(player.getX(), player.getY());
    // std::cout << std::endl;
    // std::cout << "Raycast hitbox: ";
    // player.raycast.hitbox.printBorders(player.getX(), player.getY());
    // std::cout << std::endl;

    for (Ground &ground : grounds)
    {
        ground.update();
    }
    player.updatePosition();
    collisionsManager();
    std::cout << "isFall" << player.getIsFall() << std::endl;
    std::cout << "onGround" << player.getIsOnGround() << std::endl;
    std::cout << "vy" << player.getVY() << std::endl;
    player.setNullVX();
}

void GameManager::collisionsManager()
{
    player.setOnGround(false);
    for (Ground &ground : grounds)
    {
        // CollisionResult res = sweptAABB(player.raycast.hitbox, player.getX(), player.getY(), player.getVX(), player.getVY(),
        //                                 ground.hitbox, ground.getX(), ground.getY());
        // if (res.hit && res.normalY == -1.0f)
        // {
        //     player.updateFallOnGround(ground);
        if (!player.getIsFall() && !ground.getIsBarrier())
            continue;
        if (AABB(player.raycast.hitbox, player.getX(), player.getY(),
                 ground.hitbox, ground.getX(), ground.getY()))
        {
            player.updateFallOnGround(ground);
            // std::cout << "Player hitbox: ";
            // player.hitbox.printBorders(player.getX(), player.getY());
            // std::cout << std::endl;
            // std::cout << "Raycast hitbox: ";
            // player.raycast.hitbox.printBorders(player.getX(), player.getY() + player.getHeight() / 2);
            // std::cout << std::endl;

            // std::cout << "Ground hitbox: ";
            // ground.hitbox.printBorders(ground.getX(), ground.getY());
            // std::cout << std::endl;
        }
        // else
        // {
        //     CollisionResult res = sweptAABB(player.hitbox, player.getX(), player.getY(), player.getVX(), player.getVY(),
        //                                     ground.hitbox, ground.getX(), ground.getY());
        //     if (res.hit)
        //     {
        //         std::cout << "res.hit" << res.hitY << std::endl;
        //         player.handleConflict(ground.hitbox, ground.getX(), ground.getY(), res.hitX, res.hitY);
        //     }
        //     // if (AABB(player.hitbox, player.getX(), player.getY(),
        //     //          ground.hitbox, ground.getX(), ground.getY()))
        //     // {
        //     //     player.setX(player.getPrevX());
        //     //     player.setY(player.getPrevY());
        //     // }
        // }
        else
        {
            // if (AABB(player.hitbox, player.getX(), player.getY(),
            //          ground.hitbox, ground.getX(), ground.getY()))
            // {
            //     std::cout << "Ground hitbox: ";
            //     ground.hitbox.printBorders(ground.getX(), ground.getY());
            //     std::cout << std::endl;
            //     if (player.getVX() > 0.0f)
            //     {
            //         player.setX(ground.getX() - player.getWidth());
            //         player.setNullVX();
            //     }
            //     else if (player.getVX() < 0.0f)
            //     {
            //         player.setX(ground.getX() + ground.getWidth());
            //         player.setNullVX();
            //     }
            // }
            checkCollisions(ground.hitbox, ground.getX(), ground.getY());
        }
    }
}

void GameManager::checkCollisions(HitBox &obj, float objX, float objY)
{
    float stopX = 0.0f, stopY = 0.0f;

    float curX = player.getX();
    float vx = player.getVX();
    float nextX = curX + vx;

    // while (curX != nextX && stopX == 0.0f)
    // {
    //     prevX = curX;
    //     curX += std::fabs(vx) / vx;
    //     if (AABB(player.hitbox, curX, player.getY(),
    //              obj, objX, objY))
    //     {
    //         // stopX = prevX;
    //         stopX = (vx > 0.0f) ? obj.leftB(objX) - player.getWidth() : obj.rightB(objX);
    //     }
    // }
    if (vx != 0.0f && AABB(player.hitbox, nextX, player.getY(),
                           obj, objX, objY))
    {
        // stopX = prevX;
        stopX = (vx > 0.0f) ? obj.leftB(objX) - player.getWidth() : obj.rightB(objX);
        player.handleConflict(stopX, 0.0f);
    }

    float curY = player.getY();
    float vy = player.getVY();
    float nextY = curY - vy;

    // while (curY != nextY && stopY == 0.0f)
    // {
    //     prevY = curY;
    //     curY -= std::fabs(vy) / vy;
    //     if (AABB(player.hitbox, player.getX(), curY,
    //              obj, objX, objY))
    //     {
    //         stopY = prevY;
    //     }
    // }
    if (AABB(player.hitbox, player.getX(), nextY,
             obj, objX, objY))
    {
        stopY = (vy > 0.0f) ? obj.bottomB(objY) : obj.topB(objY) - player.getHeight();
        player.handleConflict(0.0f, stopY);
    }
}
