#include "GameManager.h"
#include <3ds.h>
#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include "Ground.h"
#include "Camera.h"
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
    if (ProjectSettings::CONSOLE) {
        consoleInit(GFX_BOTTOM, NULL);
	}
	camera = Camera(0, Const::SCREEN_HEIGHT, 0.1, 0.1);


    //if (ProjectSettings::CONSOLE)
    consoleInit(GFX_BOTTOM, NULL);
    // object init
    objects.push_back(std::make_unique<LowerScreen>(0, 0, 320, 240, "romfs:/gfx/lower_screen.t3x"));
    grounds.push_back(Ground(Const::SCREEN_WIDTH / 3, Const::SCREEN_HEIGHT / 4, 5, 10, false));
    grounds.push_back(Ground(Const::SCREEN_WIDTH / 2, Const::SCREEN_HEIGHT / 4, 15, Const::SCREEN_WIDTH / 8, false, 'D', 1, 40));
    grounds.push_back(Ground(Const::SCREEN_WIDTH * 3 / 4, Const::SCREEN_HEIGHT / 4, 2, Const::SCREEN_WIDTH / 8, true, 'V', 1, 40));
    grounds.push_back(Ground(Const::SCREEN_WIDTH * 3 / 4, Const::SCREEN_HEIGHT / 2, 100, Const::SCREEN_WIDTH / 8, false));
    grounds.push_back(Ground(0.0f, Const::SCREEN_HEIGHT / 2, 30, Const::SCREEN_WIDTH / 4, false));
    grounds.push_back(Ground(0.0f, (Const::SCREEN_HEIGHT / 4) * 3, 10, Const::SCREEN_WIDTH, false));

    groundEnemies.push_back(GroundEnemy(Const::SCREEN_WIDTH / 2, Const::SCREEN_HEIGHT / 4, 60, 30, 3, 1.0f, 'M', 1.0f, 5.0f, 'W', 50.0f));

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
	float cameraPos = camera.getX();
	float playerPos = player.getX();
	std::cout << cameraPos << " " << playerPos << std::endl;
    if (playerPos - cameraPos >= 100.0) {
		camera.changeX(2.0);
	}
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C2D_TargetClear(topRight, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topRight);
    
    player.draw(cameraPos);
	
    // player.raycast.hitbox.draw(player.getX(), player.getY());

    for (Ground &ground : grounds)
        ground.draw(cameraPos);
    for (GroundEnemy &groundEnemy : groundEnemies)
        groundEnemy.draw(cameraPos);

    if (!ProjectSettings::CONSOLE)
    {
        C2D_TargetClear(botLeft, C2D_Color32(0xff, 0xff, 0xff, 0xff));
        C2D_SceneBegin(botLeft);
        for (auto &obj : objects)
            obj->draw(cameraPos);
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
    for (GroundEnemy &groundEnemy : groundEnemies)
    {
        groundEnemy.update();
        // Logger::info(groundEnemy.getLastHitX(), groundEnemy.getVX());
    }

    CollisionsManager();

    player.setNullVX();
}

void GameManager::CollisionsManager()
{
    EntityGroundCollisions(player);
    for (GroundEnemy &groundEnemy : groundEnemies)
        EntityGroundCollisions(groundEnemy);
}

void GameManager::EntityGroundCollisions(Entity &entity)
{
    Ground *plat = entity.getGroundPlatform();
    if (plat != nullptr)
    {
        float platVY = plat->getVY();
        float platVX = plat->getVX();
        entity.setY(entity.getY() - platVY);
        entity.setX(entity.getX() + platVX);
    }

    float prevX = entity.getX();
    entity.applyLR();

    for (Ground &ground : grounds)
    {
        if (!ground.getIsBarrier() && plat == nullptr)
        {
            CollisionResult res = sweptAABB(entity.hitbox, prevX, entity.getY(), entity.getVX(), entity.getVY(),
                                            ground.hitbox, ground.getX(), ground.getY());
            bool collision = AABB(entity.hitbox, entity.getX(), entity.getY(),
                                  ground.hitbox, ground.getX(), ground.getY());
            if (res.hit && res.normalX != 0.0f && collision)
            {
                resolveX(entity, ground);
            }
            continue;
        }

        if (!AABB(entity.hitbox, entity.getX(), entity.getY(),
                  ground.hitbox, ground.getX(), ground.getY()))
            continue;

        if (isHorizontalCollisionPrimary(entity, ground))
        {
            resolveX(entity, ground);
        }
    }

    entity.resetGroundPlatform();
    float prevY = entity.getY();
    entity.applyUD();
    float vy = entity.getVY();

    for (Ground &ground : grounds)
    {
        // Logger::warn("vy", vy);
        // CollisionResult res = sweptAABB(entity.raycast.hitbox, entity.getX(), prevY, entity.getVX(), vy,
        //                                 ground.hitbox, ground.getX(), ground.getY());
        // bool rayCastCollision = AABB(entity.raycast.hitbox, entity.getX(), entity.getY(),
        //                              ground.hitbox, ground.getX(), ground.getY());
        // Logger::info("Hit:", res.hit, rayCastCollision, res.normalY, vy);
        // if (res.hit && res.normalY == -1.0f && rayCastCollision)
        // {
        //     entity.landOnGround(&ground);
        //     Logger::warn("Stay on ground");
        //     continue;
        // }

        if (!AABB(entity.hitbox, entity.getX(), entity.getY(),
                  ground.hitbox, ground.getX(), ground.getY()))
            continue;

        if (vy <= 0.0f)
        {
            float prevBottom = prevY + entity.getHeight();
            float groundTop = ground.hitbox.topB(ground.getY());

            if (prevBottom <= groundTop + 2.0f)
            {
                entity.landOnGround(&ground);
            }
            else if (ground.getIsBarrier())
            {
                resolveY(entity, ground);
            }
        }
        else
        {
            if (ground.getIsBarrier())
            {
                resolveY(entity, ground);
            }
        }
    }
}

bool GameManager::isHorizontalCollisionPrimary(Entity &entity, Ground &ground)
{
    float entityLeft = entity.hitbox.leftB(entity.getX());
    float entityRight = entity.hitbox.rightB(entity.getX());
    float entityTop = entity.hitbox.topB(entity.getY());
    float entityBottom = entity.hitbox.bottomB(entity.getY());

    float groundLeft = ground.hitbox.leftB(ground.getX());
    float groundRight = ground.hitbox.rightB(ground.getX());
    float groundTop = ground.hitbox.topB(ground.getY());
    float groundBottom = ground.hitbox.bottomB(ground.getY());

    float overlapX = std::min(entityRight - groundLeft, groundRight - entityLeft);
    float overlapY = std::min(entityBottom - groundTop, groundBottom - entityTop);

    return overlapX <= overlapY;
}

void GameManager::resolveX(Entity &entity, Ground &ground)
{
    float entityLeft = entity.hitbox.leftB(entity.getX());
    float entityRight = entity.hitbox.rightB(entity.getX());
    float groundLeft = ground.hitbox.leftB(ground.getX());
    float groundRight = ground.hitbox.rightB(ground.getX());

    float overlapFromRight = entityRight - groundLeft;
    float overlapFromLeft = groundRight - entityLeft;

    if (overlapFromRight < overlapFromLeft)
    {
        entity.setLastHitX(1);
        entity.setX(entity.getX() - overlapFromRight);
    }
    else
    {
        entity.setLastHitX(-1);
        entity.setX(entity.getX() + overlapFromLeft);
    }
    entity.setNullVX();
}

void GameManager::resolveY(Entity &entity, Ground &ground)
{
    // float vy = ground.getVY();
    // if (!LeftOrRight(entity.hitbox, entity.getX(), entity.getY(),
    //                  ground.hitbox, ground.getX(), ground.getY()))
    // {
    //     entity.setY(ground.hitbox.bottomB(ground.getY()) + 1.0f);
    //     entity.setNullVY();
    // }

    float entityTop = entity.hitbox.topB(entity.getY());
    float entityBottom = entity.hitbox.bottomB(entity.getY());
    float groundTop = ground.hitbox.topB(ground.getY());
    float groundBottom = ground.hitbox.bottomB(ground.getY());

    float overlapFromBottom = entityBottom - groundTop;
    float overlapFromTop = groundBottom - entityTop;

    if (overlapFromBottom < overlapFromTop)
    {
        entity.setLastHitY(1);
        entity.setY(entity.getY() - overlapFromBottom);
    }
    else
    {
        entity.setLastHitY(-1);
        entity.setY(entity.getY() + overlapFromTop + ground.getSpeed());
    }
    entity.setNullVY();
}
