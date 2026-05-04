#include "GameManager.h"
#include <3ds.h>
#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include "Ground.h"
#include "Player.h"
#include "LowerScreen.h"
#include "Core.h"
#include "Logger.h"

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
    botLeft = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    if (ProjectSettings::CONSOLE)
        consoleInit(GFX_BOTTOM, NULL);
    // object init
    objects.push_back(std::make_unique<LowerScreen>(0, 0, 320, 240, "romfs:/gfx/lower_screen.t3x"));
    grounds.push_back(Ground(Const::SCREEN_WIDTH / 3, Const::SCREEN_HEIGHT / 4, 5, 10, false));
    grounds.push_back(Ground(Const::SCREEN_WIDTH / 2, Const::SCREEN_HEIGHT / 4, 15, Const::SCREEN_WIDTH / 8, true, 'D', 1, 40));
    grounds.push_back(Ground(Const::SCREEN_WIDTH * 3 / 4, Const::SCREEN_HEIGHT / 4, 2, Const::SCREEN_WIDTH / 8, true, 'V', 1, 40));
    grounds.push_back(Ground(0.0f, Const::SCREEN_HEIGHT / 2, 30, Const::SCREEN_WIDTH / 4, false));
    grounds.push_back(Ground(0.0f, (Const::SCREEN_HEIGHT / 4) * 3, 10, Const::SCREEN_WIDTH, false));

    groundEnemies.push_back(GroundEnemy(Const::SCREEN_WIDTH * 3 / 4, Const::SCREEN_HEIGHT / 4, 60, 30, 3, 5.0f, 'M', 1.0f, 5.0f, 'R', 5.0f));

    player = Player(0, 0, 60, 30);
}

void GameManager::exit()
{
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
    for (auto &obj : objects)
        obj->freeSheet();
}

void GameManager::draw()
{
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C2D_TargetClear(topRight, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topRight);

    player.draw();
    // player.raycast.hitbox.draw(player.getX(), player.getY());

    for (Ground &ground : grounds)
        ground.draw();
    for (GroundEnemy &groundEnemy : groundEnemies)
        groundEnemy.draw();

    if (!ProjectSettings::CONSOLE)
    {
        C2D_TargetClear(botLeft, C2D_Color32(0xff, 0xff, 0xff, 0xff));
        C2D_SceneBegin(botLeft);
        for (auto &obj : objects)
            obj->draw();
    }

    C3D_FrameEnd(0);
}

void GameManager::update(int &s)
{
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

    bool isJumpButtonDown = (kHeld & KEY_A) ? true : false;

    player.update(isJumpButtonDown);

    for (Ground &ground : grounds)
        ground.update();

    PlayerGroundCollisionsManager();
    player.setNullVX();
}

void GameManager::PlayerGroundCollisionsManager()
{

    Ground *plat = player.getGroundPlatform();
    if (plat != nullptr)
    {
        float platVY = plat->getVY();
        float platVX = plat->getVX();
        player.setY(player.getY() - platVY);
        player.setX(player.getX() + platVX);
    }

    float prevX = player.getX();
    player.moveLR();

    for (Ground &ground : grounds)
    {
        if (!ground.getIsBarrier() && player.getGroundPlatform() == nullptr)
        {
            CollisionResult res = sweptAABB(player.hitbox, prevX, player.getY(), player.getVX(), player.getVY(),
                                            ground.hitbox, ground.getX(), ground.getY());
            bool collision = AABB(player.hitbox, player.getX(), player.getY(),
                                  ground.hitbox, ground.getX(), ground.getY());
            if (res.hit && res.normalX != 0.0f && collision)
            {
                resolveX(player, ground);
            }
            continue;
        }

        if (!AABB(player.hitbox, player.getX(), player.getY(),
                  ground.hitbox, ground.getX(), ground.getY()))
            continue;

        if (isHorizontalCollisionPrimary(ground))
            resolveX(player, ground);
    }

    player.resetGroundState();
    float prevY = player.getY();
    player.moveUD();

    for (Ground &ground : grounds)
    {
        float vy = player.getVY();
        // Logger::warn("vy", vy);
        // CollisionResult res = sweptAABB(player.raycast.hitbox, player.getX(), prevY, player.getVX(), vy,
        //                                 ground.hitbox, ground.getX(), ground.getY());
        // bool rayCastCollision = AABB(player.raycast.hitbox, player.getX(), player.getY(),
        //                              ground.hitbox, ground.getX(), ground.getY());
        // Logger::info("Hit:", res.hit, rayCastCollision, res.normalY, vy);
        // if (res.hit && res.normalY == -1.0f && rayCastCollision)
        // {
        //     player.landOnGround(&ground);
        //     Logger::warn("Stay on ground");
        //     continue;
        // }

        if (!AABB(player.hitbox, player.getX(), player.getY(),
                  ground.hitbox, ground.getX(), ground.getY()))
            continue;

        if (vy <= 0.0f)
        {
            float prevBottom = prevY + player.getHeight();
            float groundTop = ground.hitbox.topB(ground.getY());

            if (prevBottom <= groundTop + 2.0f)
            {
                player.landOnGround(&ground);
            }
            else if (ground.getIsBarrier())
            {
                resolveY(ground);
            }
        }
        else
        {
            if (ground.getIsBarrier())
            {
                resolveY(ground);
            }
        }
    }
}

bool GameManager::isHorizontalCollisionPrimary(Ground &ground)
{
    float playerLeft = player.hitbox.leftB(player.getX());
    float playerRight = player.hitbox.rightB(player.getX());
    float playerTop = player.hitbox.topB(player.getY());
    float playerBottom = player.hitbox.bottomB(player.getY());

    float groundLeft = ground.hitbox.leftB(ground.getX());
    float groundRight = ground.hitbox.rightB(ground.getX());
    float groundTop = ground.hitbox.topB(ground.getY());
    float groundBottom = ground.hitbox.bottomB(ground.getY());

    float overlapX = std::min(playerRight - groundLeft, groundRight - playerLeft);
    float overlapY = std::min(playerBottom - groundTop, groundBottom - playerTop);

    return overlapX <= overlapY;
}

void GameManager::resolveX(Entity &entity, Ground &ground)
{
    float playerLeft = entity.hitbox.leftB(entity.getX());
    float playerRight = entity.hitbox.rightB(entity.getX());
    float groundLeft = ground.hitbox.leftB(ground.getX());
    float groundRight = ground.hitbox.rightB(ground.getX());

    float overlapFromRight = playerRight - groundLeft;
    float overlapFromLeft = groundRight - playerLeft;

    if (overlapFromRight < overlapFromLeft)
    {
        entity.setX(entity.getX() - overlapFromRight);
    }
    else
    {
        entity.setX(entity.getX() + overlapFromLeft);
    }
    entity.setNullVX();
}

void GameManager::resolveY(Ground &ground)
{
    player.setY(ground.hitbox.bottomB(ground.getY()) + 1.0f);
    player.setNullVY();
}
