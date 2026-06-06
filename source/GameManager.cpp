#include "GameManager.h"
#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <vector>
#include <limits>
#include <citro2d.h>

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
    {
        consoleInit(GFX_BOTTOM, NULL);
    }
    heartImg = C2D_SpriteSheetGetImage(C2D_SpriteSheetLoad("romfs:/gfx/hp.t3x"), 0);
    camera = Camera(0, Const::SCREEN_HEIGHT, 0.1, 0.1, 0);
    camera.setFrameSpeed(15);
    pointer = Pointer(0, 0, 0.01, 0.01);
    marker = Marker(34, 149, 0, 0, "romfs:/gfx/marker.t3x");
    marker.setMaxPos(700);
    ls = LowerScreen(0, 0, 320, 240, "romfs:/gfx/lower_screen.t3x");
    grounds.emplace_back(Const::SCREEN_WIDTH / 3, Const::SCREEN_HEIGHT / 4, 5, 10, false, GroundMode::Static);
    grounds.emplace_back(Const::SCREEN_WIDTH / 2, Const::SCREEN_HEIGHT / 4, 15, Const::SCREEN_WIDTH / 8, false, GroundMode::Descent, 40, 1);
    grounds.emplace_back(Const::SCREEN_WIDTH * 3 / 4, Const::SCREEN_HEIGHT / 4, 2, Const::SCREEN_WIDTH / 8, true, GroundMode::Vertical, 40, 1);
    grounds.emplace_back(Const::SCREEN_WIDTH * 3 / 4, Const::SCREEN_HEIGHT / 2, 100, Const::SCREEN_WIDTH / 8, false, GroundMode::Static);
    grounds.emplace_back(0.0f, Const::SCREEN_HEIGHT / 2, 30, Const::SCREEN_WIDTH / 4, false, GroundMode::Static);
    grounds.emplace_back(0.0f, (Const::SCREEN_HEIGHT / 4) * 3, 10, Const::SCREEN_WIDTH, false, GroundMode::Static);

    groundEnemies.emplace_back(Const::SCREEN_WIDTH / 2, Const::SCREEN_HEIGHT / 4, 60, 30, 3, 1.0f, ProjectSettings::FPS * 2, AttackType::Shot, 100.0f, 1000.0f, EnemyPatrolType::WallToWall, 50.0f);

    powerups.emplace_back(Const::SCREEN_WIDTH / 2, Const::SCREEN_HEIGHT / 2, Const::POWERUP_SIZE, Const::POWERUP_SIZE, AttackType::Shot, 5 * ProjectSettings::FPS);
    powerups.emplace_back(Const::SCREEN_WIDTH / 2 + 50, Const::SCREEN_HEIGHT / 2, Const::POWERUP_SIZE, Const::POWERUP_SIZE, AttackType::Sword, 10 * ProjectSettings::FPS);

    player = Player(0, 0);
}

void GameManager::exit()
{
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
    ls.freeSheet();
}

void GameManager::draw()
{
    float cameraPos = camera.getX();
    float playerPos = player.getX();
    float dx = playerPos - cameraPos;
    float cameraSpeed = 0.0f;
    if (dx >= 100.0)
    {
        cameraSpeed = dx / camera.getFrameSpeed();
        camera.changeX(cameraSpeed);
    }
    else if (dx <= 50) {
        cameraSpeed = dx / camera.getFrameSpeed();
        camera.changeX(cameraSpeed);
    }

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C2D_TargetClear(topRight, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topRight);

    player.draw(cameraPos, 1);

    // player.raycast.hitbox.draw(player.getX(), player.getY());

    for (Ground &ground : grounds)
        ground.draw(cameraPos, 0);
    for (GroundEnemy &groundEnemy : groundEnemies)
    {
        groundEnemy.draw(cameraPos, 1);
    }
    int i = 0;
    for (Projectile &p : projectiles)
    {
        p.draw(cameraPos, 1);
        i++;
    }
    for (Powerup &pu : powerups)
    {
        if (!pu.getIsPickedUp())
            pu.draw(cameraPos, 0);
    }

    if (!ProjectSettings::CONSOLE)
    {
        C2D_TargetClear(botLeft, C2D_Color32(0xff, 0xff, 0xff, 0xff));
        C2D_SceneBegin(botLeft);
        int playerHp = player.getHP();
        std::vector<Powerup> &currentPowerups = player.getPowerups();
        int s = currentPowerups.size();
        if (s != 0)
        {
            for (int i = 0; i < s; i++)
            {
                currentPowerups[i].draw(0, 1);
            }
        }
        ls.draw(cameraPos, 0);
        marker.draw(cameraPos, 1);
        for (int i = 0; i < playerHp; i++) {
            C2D_DrawImageAt(heartImg, 182 + 40*i, 20, 0);
        }
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
    if (kDown & KEY_B)
        player.startAttack(player.getAttackType(), timer, nearestEnemyX, nearestEnemyY);

    if (kHeld & KEY_TOUCH)
    {
        hidTouchRead(&touch);
        pointer.setX(touch.px);
        pointer.setY(touch.py);
        std::vector<Powerup> &playerPowerups = player.getPowerups();

        for (size_t i = 0; i < playerPowerups.size(); i++)
        {
            Powerup &pu = playerPowerups[i];

            if (AABB(pointer.hitbox, pointer.getX(), pointer.getY(),
                     pu.clickHitBox, pu.getX(), pu.getY()))
            {
                pu.use(timer);
                player.usePowerup(i);
                break;
            }
        }
    }
    bool isJumpButtonDown = (kHeld & KEY_A) ? true : false;
    for (Ground &ground : grounds)
        ground.update(timer);

    float minDist = std::numeric_limits<float>::max();
    nearestEnemyX = Const::DEFAULT_X;
    nearestEnemyY = Const::DEFAULT_Y;
    for (GroundEnemy &groundEnemy : groundEnemies)
    {
        groundEnemy.update(projectiles, player.getCentreX(), player.getCentreY(), timer);
        Logger::info(groundEnemy.getHP());
        float enemyX = groundEnemy.getCentreX();
        if (player.isObjForward(enemyX))
        {
            float enemyY = groundEnemy.getCentreY();
            float dist = player.distToObj(enemyX, enemyY);
            if (dist < minDist)
            {
                minDist = dist;
                nearestEnemyX = enemyX;
                nearestEnemyY = enemyY;
            }
        }
    }
    attackManager();
    for (Projectile &p : projectiles)
    {
        p.update();
    }
    for (Powerup &pu : powerups)
        pu.update(timer);
    for (Powerup &pu : player.getPowerups())
        pu.update(timer);
    marker.update(player.getX());
    player.updatePowerup(timer);
    player.update(isJumpButtonDown);
    collisionsManager();
    eraseManager();
}


void GameManager::attackManager()
{
    PendingAttack attack;

    if (player.consumeReadyAttack(timer, attack))
    {
        resolveAttack(player, OwnerType::Player, attack);
    }

    for (GroundEnemy &enemy : groundEnemies)
    {
        if (enemy.consumeReadyAttack(timer, attack))
        {
            resolveAttack(enemy, OwnerType::Enemy, attack);
        }
    }
}

void GameManager::resolveAttack(Entity &attacker, OwnerType owner, PendingAttack attack)
{
    float projectileSpawnX = (attack.view == 1) ? attacker.getX() + attacker.getWidth() : attacker.getX();

    float projectileSpawnY = attacker.getY() + attacker.getHeight() / 4;

    switch (attack.type)
    {
    case AttackType::Bubble:
        projectileSpawnX -= Const::BUBBLE_SIZE / 2;
        projectiles.emplace_back(projectileSpawnX, projectileSpawnY, 10.0f, 10.0f, 1.0f, owner, timer, AttackType::Bubble, attack.view, 5);
        break;

    case AttackType::Shot:
        projectileSpawnX -= Const::SHOT_SIZE / 2;
        if (attack.targetX == Const::DEFAULT_X && attack.targetY == Const::DEFAULT_Y && owner == OwnerType::Player)
        {
            attack.targetX = (attack.view == 1) ? attacker.getX() + attacker.getWidth() + Const::FIX_CONST : attacker.getX() - Const::FIX_CONST;
            attack.targetY = attacker.getY() + attacker.getHeight() / 4;
        }
        projectiles.emplace_back(projectileSpawnX, projectileSpawnY, 5.0f, 5.0f, 6.0f, owner, timer, AttackType::Shot, attack.targetX, attack.targetY);
        break;

    case AttackType::Sword:
        resolveSwordAttack(attacker, owner, attack.view);
        break;
    }
}

void GameManager::resolveSwordAttack(Entity &attacker, OwnerType owner, int view)
{
    if (owner == OwnerType::Player)
    {
        for (GroundEnemy &enemy : groundEnemies)
        {
            if (isInSwordArc(attacker, enemy, view))
                enemy.subHP();
        }
    }
    else
    {
        if (isInSwordArc(attacker, player, view))
            player.subHP();
    }
}

bool GameManager::isInSwordArc(Entity &attacker, Entity &target, int view)
{
    if (!attacker.isObjForward(target.getCentreX()))
        return false;

    float centreX = attacker.getCentreX();
    float centreY = attacker.getCentreY();

    float left = target.hitbox.leftB(target.getX());
    float right = target.hitbox.rightB(target.getX());
    float top = target.hitbox.topB(target.getY());
    float bottom = target.hitbox.bottomB(target.getY());

    float closestX = std::clamp(centreX, left, right);
    float closestY = std::clamp(centreY, top, bottom);

    float dx = closestX - centreX;
    float dy = closestY - centreY;

    return std::powf(dx, 2.0f) + std::powf(dy, 2.0f) <= std::powf(Const::SWORD_RADIUS, 2.0f);
}

void GameManager::collisionsManager()
{
    entityGroundCollisions(player);
    for (Projectile &p : projectiles)
        projectileCollisions(p);

    for (GroundEnemy &groundEnemy : groundEnemies)
    {
        entityGroundCollisions(groundEnemy);
        groundEnemy.setNullVX();
    }
    for (Powerup &pu : powerups)
    {
        powerupCollisions(&pu);
    }
    player.setNullVX();
}

void GameManager::eraseManager()
{
    groundEnemies.erase(
        std::remove_if(groundEnemies.begin(), groundEnemies.end(),
                       [](const GroundEnemy &groundEnemy)
                       {
                           return groundEnemy.getIsDead();
                       }),
        groundEnemies.end());

    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
                       [](const Projectile &p)
                       {
                           return p.getIsDead();
                       }),
        projectiles.end());
    powerups.erase(
        std::remove_if(powerups.begin(), powerups.end(),
                       [](const Powerup &pu)
                       {
                           return pu.getIsDead();
                       }),
        powerups.end());
    player.erasePowerups();
}

void GameManager::entityGroundCollisions(Entity &entity)
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

void GameManager::projectileCollisions(Projectile &p)
{
    for (Ground &ground : grounds)
    {
        if ((AABB(p.hitbox, p.getX(), p.getY(),
                  ground.hitbox, ground.getX(), ground.getY())))
        {
            p.setIsDead(true);
            return;
        }
    }
    switch (p.getOwner())
    {
    case OwnerType::Enemy:
        if ((AABB(p.hitbox, p.getX(), p.getY(),
                  player.hitbox, player.getX(), player.getY())))
        {
            p.setIsDead(true);
            player.subHP();

        }
        break;
    case OwnerType::Player:
        for (GroundEnemy &groundEnemy : groundEnemies)
        {
            if ((AABB(p.hitbox, p.getX(), p.getY(),
                      groundEnemy.hitbox, groundEnemy.getX(), groundEnemy.getY())))
            {
                p.setIsDead(true);
                groundEnemy.subHP();
            }
        }
        break;

    default:
        break;
    }
}

void GameManager::powerupCollisions(Powerup *pu)
{
    if ((AABB(pu->hitbox, pu->getX(), pu->getY(),
              player.hitbox, player.getX(), player.getY())))
    {
        std::vector<Powerup>::iterator it = powerups.begin() + (pu - powerups.data());
        pu->pickUp();
        player.pickUpPowerup(it);
        powerups.erase(it);
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
