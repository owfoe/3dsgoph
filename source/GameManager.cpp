#include "GameManager.h"

#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <vector>
#include <limits>
#include <citro2d.h>
#include <dirent.h>

#include "../include/Core.h"

GameManager::GameManager(int s) {}

void GameManager::init()
{
    // system init
    romfsInit();
    cfguInit();
    gfxInitDefault();
    gfxSet3D(true);

    // gfx
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    // text system init
    g_staticBuf = C2D_TextBufNew(4096);
    customFont = C2D_FontLoad(Path::CUSTOM_FONT);
    mapTextBuf = C2D_TextBufNew(4096);

    // screen target init
    topRight = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
    botLeft = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    topLeft = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    // console init
    if (ProjectSettings::CONSOLE)
    {
        consoleInit(GFX_BOTTOM, NULL);
    }
    flyEnemies.reserve(50);
    // sheets init
    hpSheet = C2D_SpriteSheetLoad(Path::HP_SHEET);
    gameOver1Sheet = C2D_SpriteSheetLoad(Path::GAME_OVER1_SHEET);
    gameOver2Sheet = C2D_SpriteSheetLoad(Path::GAME_OVER2_SHEET);
    gameOver3Sheet = C2D_SpriteSheetLoad(Path::GAME_OVER3_SHEET);
    logoSheet = C2D_SpriteSheetLoad(Path::LOGO_SHEET);
    menu1Sheet = C2D_SpriteSheetLoad(Path::MENU1_SHEET);
    menu2Sheet = C2D_SpriteSheetLoad(Path::MENU2_SHEET);
    menu3Sheet = C2D_SpriteSheetLoad(Path::MENU3_SHEET);
    lowerMenuSheet = C2D_SpriteSheetLoad(Path::LOWER_MENU_SHEET);
    enemyFlySheet = C2D_SpriteSheetLoad(Path::ENEMY_FLY_SHEET);

    // images init
    heartImg = C2D_SpriteSheetGetImage(hpSheet, 0);
    gameOver1Img = C2D_SpriteSheetGetImage(gameOver1Sheet, 0);
    gameOver2Img = C2D_SpriteSheetGetImage(gameOver2Sheet, 0);
    gameOver3Img = C2D_SpriteSheetGetImage(gameOver3Sheet, 0);
    logoImg = C2D_SpriteSheetGetImage(logoSheet, 0);
    menu1Img = C2D_SpriteSheetGetImage(menu1Sheet, 0);
    menu2Img = C2D_SpriteSheetGetImage(menu2Sheet, 0);
    menu3Img = C2D_SpriteSheetGetImage(menu3Sheet, 0);
    lowerMenuImg = C2D_SpriteSheetGetImage(lowerMenuSheet, 0);

    // util objects init
    camera = Camera(0, Const::SCREEN_HEIGHT, 0.1, 0.1, 0.15f);
    camera.setFrameSpeed(15);
    pointer = Pointer(0, 0, 0.01, 0.01);
    marker = Marker(Const::MARKER_START_X, 149, 0, 0, Path::MARKER);

    ls = LowerScreen(0, 0, 320, 240, Path::LOWER_SCREEN);

    for (std::string &fileName : getFiles(Path::MAPS))
        loadMap(fileName);
    currentMap = mapTitles[0];
    currentMapInd = 0;
    mapCount = maps.size();
    createMap();

    offset1 = 10;
    offset2 = 5;
    offset3 = 0;

    // text init!! a lot of stuff
    textInit();

    microphoneReady = microphone.init();
}

void GameManager::exit()
{
    ls.freeSheet();
    marker.freeSheet();
    //player.freeSheet();
    player.animator.exit();
    projectiles.clear();
    flyEnemies.clear();

    for (Powerup &pu : powerups)
        pu.freeSheet();

    if (microphoneReady)
        microphone.exit();

    C2D_SpriteSheetFree(hpSheet);
    C2D_SpriteSheetFree(logoSheet);
    C2D_SpriteSheetFree(gameOver1Sheet);
    C2D_SpriteSheetFree(gameOver2Sheet);
    C2D_SpriteSheetFree(gameOver3Sheet);
    C2D_SpriteSheetFree(menu1Sheet);
    C2D_SpriteSheetFree(menu2Sheet);
    C2D_SpriteSheetFree(menu3Sheet);
    C2D_SpriteSheetFree(lowerMenuSheet);
    C2D_SpriteSheetFree(enemyFlySheet);
    C2D_TextBufDelete(g_staticBuf);
    C2D_TextBufDelete(mapTextBuf);
    C2D_FontFree(customFont);

    C2D_Fini();
    C3D_Fini();
    romfsExit();
    cfguExit();
    gfxExit();
}

void GameManager::draw()
{
    switch (getState()) {
        case GameManagerState::Load:
            loadDraw();
            break;
        case GameManagerState::Title:
            titleDraw();
            break;
        case GameManagerState::Maps:
            mapsDraw();
            break;
        case GameManagerState::GameOver:
            gameOverDraw();
            break;
        case GameManagerState::Game:
            gameDraw();
            break;
    }
}

void GameManager::update(int &s)
{
    hidScanInput();
    kDown = hidKeysDown();
    kHeld = hidKeysHeld();
    if (kDown & KEY_SELECT) {
        player.animator.exit();
        loadSwitch(GameManagerState::Title);
        for (auto &p : flyEnemies)
            p.animator.exit();
    }

    if (kDown & KEY_START)
        s = -1;
    slider = osGet3DSliderState();

    switch (getState()) {
        case GameManagerState::Load:
            loadUpdate(s);
            break;
        case GameManagerState::Title:
            titleUpdate(s);
            break;
        case GameManagerState::Maps:
            mapsUpdate();
            break;
        case GameManagerState::GameOver:
            if (kDown)
            {
                loadSwitch(GameManagerState::Title);
            }
            break;
        case GameManagerState::Game:
            gameUpdate();
            break;
    }
}

void GameManager::updateCamera()
{
    float playerX = player.getCentreX() - camera.getX();
    float cameraX = camera.getX();

    if (playerX < Const::CAMERA_LEFT_BORDER)
        cameraX = player.getCentreX() - Const::CAMERA_LEFT_BORDER;

    else if (playerX > Const::CAMERA_RIGHT_BORDER)
        cameraX = player.getCentreX() - Const::CAMERA_RIGHT_BORDER;

    float maxCameraX = std::max(0.0f, mapWidth - Const::SCREEN_WIDTH);
    cameraX = std::clamp(cameraX, 0.0f, maxCameraX);
    float difference = cameraX - camera.getX();
    camera.changeX(difference * camera.getSmoothing());
    camera.setX(std::clamp(camera.getX(), 0.0f, maxCameraX));
}

void GameManager::loadMap(std::string fileName)
{
    if (maps.find(fileName) == maps.end())
    {
        MapData map;
        if (!MapLoader::load(Path::MAPS + fileName, map))
            return;
        std::string title = map.title;
        mapTitles.emplace_back(title);
        maps.emplace(title, std::move(map));
    }
}

void GameManager::createMap()
{
    if (currentMap.empty())
        return;

    MapData &map = maps.at(currentMap);

    grounds.clear();
    groundEnemies.clear();
    flyEnemies.clear();
    projectiles.clear();
    powerups.clear();

    grounds.reserve(map.grounds.size());
    groundEnemies.reserve(map.groundEnemies.size());
    flyEnemies.reserve(map.flyEnemies.size());
    powerups.reserve(map.powerups.size());

    for (GroundData &data : map.grounds)
        grounds.emplace_back(data.x, data.y, data.height, data.width, data.isBarrier, data.mode, data.radius, data.speed);

    for (EnemyData &data : map.groundEnemies)
        groundEnemies.emplace_back(data.x, data.y, data.height, data.width, data.hp, data.speed, data.cooldown, data.attackType,
                                   data.aggrRadius, data.attackRadius, data.patrolType, data.patrolRadius);

    for (EnemyData &data : map.flyEnemies)
        flyEnemies.emplace_back(data.x, data.y, data.height, data.width, data.hp, data.speed, data.cooldown, data.attackType,
                                data.aggrRadius, data.attackRadius, enemyFlySheet, data.patrolType, data.patrolRadius);

    for (PowerupData &data : map.powerups)
        powerups.emplace_back(data.x, data.y, data.height, data.width, data.attackType, data.duration);

    player = Player(map.player.x, map.player.y);
    mapWidth = map.width;
    timer = 0;
    playerHp = player.getHP();
    nearestEnemyX = Const::DEFAULT_X;
    nearestEnemyY = Const::DEFAULT_Y;
    isJumpButtonDown = false;
    camera.setX(0.0f);
    marker.setX(Const::MARKER_START_X);
    marker.setMaxPos(mapWidth);
}

std::vector<std::string> GameManager::getFiles(const std::string &folder)
{
    std::vector<std::string> files;

    DIR *dir = opendir(folder.c_str());
    if (!dir)
        return files;

    dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string name = entry->d_name;

        if (name != "." && name != "..")
            files.push_back(name);
    }

    closedir(dir);
    return files;
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
    for (FlyEnemy &enemy : flyEnemies)
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
        projectiles.emplace_back(std::make_unique<Projectile>(projectileSpawnX, projectileSpawnY, 10.0f, 10.0f, 1.0f, owner, timer,
            AttackType::Bubble, attack.view, attack.power, attack.heavyBubble));
        break;

    case AttackType::Shot:
        projectileSpawnX -= Const::SHOT_SIZE / 2;
        if (attack.targetX == Const::DEFAULT_X && attack.targetY == Const::DEFAULT_Y && owner == OwnerType::Player)
        {
            attack.targetX = (attack.view == 1) ? attacker.getX() + attacker.getWidth() + Const::FIX_CONST : attacker.getX() - Const::FIX_CONST;
            attack.targetY = attacker.getY() + attacker.getHeight() / 4;
        }
        projectiles.emplace_back(std::make_unique<Projectile>(projectileSpawnX, projectileSpawnY, 9.0f, 9.0f, 6.0f, owner, timer,
            AttackType::Shot, attack.targetX, attack.targetY));
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
        for (FlyEnemy &enemy : flyEnemies)
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
    for (auto& p : projectiles)
        projectileCollisions(*p);
    entityGroundCollisions(player);
    for (GroundEnemy &groundEnemy : groundEnemies)
    {
        entityGroundCollisions(groundEnemy);
        groundEnemy.setNullVX();
    }
    for (FlyEnemy &flyEnemy : flyEnemies)
    {
        entityGroundCollisions(flyEnemy);
        flyEnemy.setNullVX();
        flyEnemy.setNullVY();
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
    flyEnemies.erase(
        std::remove_if(flyEnemies.begin(), flyEnemies.end(),
                       [](const FlyEnemy &flyEnemy)
                       {
                           return flyEnemy.getIsDead();
                       }),
        flyEnemies.end());

    projectiles.erase(
    std::remove_if(projectiles.begin(), projectiles.end(),
        [](const std::unique_ptr<Projectile>& p)
        {
            return p->getIsDead();
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

    float maxEntityX = mapWidth - entity.getWidth();
    entity.setX(std::clamp(entity.getX(), 0.0f, maxEntityX));

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
        for (FlyEnemy &flyEnemy : flyEnemies)
        {
            if ((AABB(p.hitbox, p.getX(), p.getY(),
                      flyEnemy.hitbox, flyEnemy.getX(), flyEnemy.getY())))
            {
                p.setIsDead(true);
                flyEnemy.subHP();
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

void GameManager::loadSwitch(GameManagerState ns) {
    setState(GameManagerState::Load);
    switchTimer = timer;
    nextState = ns;
}

void GameManager::loadDraw() {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(botLeft, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(botLeft);

    C2D_TargetClear(topRight, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topRight);

    C2D_TargetClear(topLeft, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topLeft);


    C3D_FrameEnd(0);
}

void GameManager::titleDraw() {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C2D_TargetClear(botLeft, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(botLeft);
    C2D_DrawImageAt(lowerMenuImg, 0, 0, 0);
    C2D_DrawText(&g_staticText[7], C2D_WithColor + C2D_AlignCenter, 160.0f, 105.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
    C2D_DrawText(&g_staticText[8], C2D_WithColor + C2D_AlignCenter, 160.0f, 135.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));

    {
        C2D_TargetClear(topLeft, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(topLeft);
        target = -1;
        drawTitleHelp(target);
    }

    {
        C2D_TargetClear(topRight, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(topRight);
        target = 1;
        drawTitleHelp(target);
    }

    C3D_FrameEnd(0);
}

void GameManager::mapsDraw() {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C2D_TargetClear(topRight, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topRight);
    target = 1;
    drawMapsHelp(target);

    C2D_TargetClear(topLeft, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topLeft);
    target = -1;
    drawMapsHelp(target);

    C2D_TargetClear(botLeft, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(botLeft);
    C2D_DrawImageAt(lowerMenuImg, 0, 0, 0);
    C2D_DrawText(&g_staticText[7], C2D_WithColor + C2D_AlignCenter, 160.0f, 105.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
    C2D_DrawText(&g_staticText[9], C2D_WithColor + C2D_AlignCenter, 160.0f, 135.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));

    C3D_FrameEnd(0);
}

void GameManager::gameOverDraw() {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C2D_TargetClear(topRight, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topRight);
    target = 1;
    drawGameOverHelp(target);

    C2D_TargetClear(topLeft, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topLeft);
    target = -1;
    drawGameOverHelp(target);

    C2D_TargetClear(botLeft, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(botLeft);
    C2D_DrawImageAt(lowerMenuImg, 0, 0, 0);
    C2D_DrawText(&g_staticText[3], C2D_WithColor + C2D_AlignCenter, 160.0f, 90.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
    C2D_DrawText(&g_staticText[4], C2D_WithColor + C2D_AlignCenter, 160.0f, 120.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
    C2D_DrawText(&g_staticText[5], C2D_WithColor + C2D_AlignCenter, 160.0f, 150.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));

    C3D_FrameEnd(0);
}

void GameManager::gameDraw() {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    C2D_TargetClear(topRight, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topRight);
    drawGameHelp();


    C2D_TargetClear(topLeft, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(topLeft);
    drawGameHelp();

    if (!ProjectSettings::CONSOLE)
    {
        C2D_TargetClear(botLeft, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(botLeft);
        std::vector<Powerup> &currentPowerups = player.getPowerups();
        powerupSize = currentPowerups.size();

        if (powerupSize != 0)
        {
            for (int i = 0; i < powerupSize; i++)
            {
                currentPowerups[i].draw(0, 1);
            }
        }
        ls.draw(cameraPos, 0);
        marker.draw(cameraPos, 1);
        for (int i = 0; i < playerHp; i++)
        {
            C2D_DrawImageAt(heartImg, 182 + 40 * i, 20, 0);
        }
    }
    C3D_FrameEnd(0);
}

void GameManager::loadUpdate(int &s) {
    if (timer - switchTimer > 60) {
        switch (nextState) {
            case GameManagerState::Game:
                setState(GameManagerState::Game);
                break;
            case GameManagerState::GameOver:
                setState(GameManagerState::GameOver);
                break;
            case GameManagerState::Title:
                setState(GameManagerState::Title);
                break;
            case GameManagerState::Maps:
                setState(GameManagerState::Maps);
                break;

            default:
                s = -1;
        }
    }
}

void GameManager::titleUpdate(int &s) {
    if (kDown & KEY_DOWN)
    {
        menuSelect += 1;
        if (menuSelect > maxSelect) menuSelect = 1;
    }
    else if (kDown & KEY_UP)
    {
        menuSelect -= 1;
        if (menuSelect < 1) menuSelect = maxSelect;
    }

    if (kDown & KEY_A)
    {
        switch (menuSelect)
        {
            case 1:
                createMap();
                loadSwitch(GameManagerState::Game);
                break;

            case 2:
                mapSelect = 0;
                mapScroll = 0;
                loadSwitch(GameManagerState::Maps);
                break;

            case 3:
                s = -1;
                break;
        }
    }
}

void GameManager::mapsUpdate() {
    if (mapCount > 0)
    {
        if (kDown & KEY_DOWN)
        {
            mapSelect++;

            if (mapSelect >= mapCount)
                    mapSelect = 0;
        }
        else if (kDown & KEY_UP)
        {
            mapSelect--;

            if (mapSelect < 0)
                mapSelect = mapCount - 1;
        }
        if (mapSelect < mapScroll)
        {
            mapScroll = mapSelect;
        }
        else if (mapSelect >= mapScroll + visibleMapCount)
        {
            mapScroll = mapSelect - visibleMapCount + 1;
        }

        if (kDown & KEY_B) loadSwitch(GameManagerState::Title);
        if (kDown & KEY_A)
        {
            currentMap = mapTitles[mapSelect];
            std::vector<std::string>::iterator it = std::find(mapTitles.begin(), mapTitles.end(), currentMap);
            currentMapInd = it - mapTitles.begin();
            createMap();
            menuSelect = 1;
            loadSwitch(GameManagerState::Title);
        }
    }
}

void GameManager::gameUpdate() {
    playerHp = player.getHP();
    cameraPos = camera.getX();

    if (playerHp == 0) {
        player.animator.exit();
        for (auto &p : flyEnemies)
            p.animator.exit();
        loadSwitch(GameManagerState::GameOver);
        return;
    }

    if (kHeld & KEY_LEFT) {
        player.moveLeft();
        player.setScaleX(-1.0f);
    }
    if (kHeld & KEY_RIGHT) {
        player.moveRight();
        player.setScaleX(1.0f);
    }
    if (kDown & KEY_A)
        player.jump();
    if (microphoneReady)
    {
        microphone.update();

        if (microphone.consumeBlow())
        {
            float strength = microphone.getLevel();
            bool isKDown = (kHeld & KEY_DOWN);

            player.startAttack(
                player.getAttackType(),
                timer,
                nearestEnemyX,
                nearestEnemyY,
                isKDown,
                strength);
        }
    }

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
    isJumpButtonDown = (kHeld & KEY_A) ? true : false;
    for (Ground &ground : grounds)
       ground.update(timer);

    float minDist = std::numeric_limits<float>::max();
    nearestEnemyX = Const::DEFAULT_X;
    nearestEnemyY = Const::DEFAULT_Y;
    for (GroundEnemy &groundEnemy : groundEnemies)
    {
        groundEnemy.update(player.getCentreX(), player.getCentreY(), timer);
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
    for (FlyEnemy &flyEnemy : flyEnemies)
    {
        flyEnemy.update(player.getCentreX(), player.getCentreY(), timer);
        float enemyX = flyEnemy.getCentreX();
        if (player.isObjForward(enemyX))
        {
            float enemyY = flyEnemy.getCentreY();
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
    for (auto& p : projectiles)
    {
        p->update();
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
    updateCamera();
    cameraPos = camera.getX();
}

void GameManager::textInit() {
    C2D_TextFontParse(&g_staticText[0], customFont, g_staticBuf, "START GAME");
    C2D_TextFontParse(&g_staticText[1], customFont, g_staticBuf, "EXIT");
    C2D_TextFontParse(&g_staticText[2], customFont, g_staticBuf, "#");
    C2D_TextFontParse(&g_staticText[3], customFont, g_staticBuf, "GAME OVER!");
    C2D_TextFontParse(&g_staticText[4], customFont, g_staticBuf, "PRESS ANY KEY");
    C2D_TextFontParse(&g_staticText[5], customFont, g_staticBuf, "TO START AGAIN");
    C2D_TextFontParse(&g_staticText[6], customFont, g_staticBuf, "MAPS");
    C2D_TextFontParse(&g_staticText[7], customFont, g_staticBuf, "PLEASE, SELECT");
    C2D_TextFontParse(&g_staticText[8], customFont, g_staticBuf, "AN OPTION.");
    C2D_TextFontParse(&g_staticText[9], customFont, g_staticBuf, "A MAP.");

    C2D_TextOptimize(&g_staticText[0]);
    C2D_TextOptimize(&g_staticText[1]);
    C2D_TextOptimize(&g_staticText[2]);
    C2D_TextOptimize(&g_staticText[3]);
    C2D_TextOptimize(&g_staticText[4]);
    C2D_TextOptimize(&g_staticText[5]);
    C2D_TextOptimize(&g_staticText[6]);
    C2D_TextOptimize(&g_staticText[7]);
    C2D_TextOptimize(&g_staticText[8]);
    C2D_TextOptimize(&g_staticText[9]);

}

void GameManager::drawGameHelp() {
    player.draw(cameraPos, 0);

    for (Ground &ground : grounds)
        ground.draw(cameraPos, 0);
    for (GroundEnemy &groundEnemy : groundEnemies)
    {
        groundEnemy.draw(cameraPos, 0);
    }
    for (FlyEnemy &flyEnemy : flyEnemies)
    {
        flyEnemy.draw(cameraPos, 0);
    }
    for (auto& p : projectiles)
    {
        p->draw(cameraPos, 1);
    }
    for (Powerup &pu : powerups)
    {
        if (!pu.getIsPickedUp())
            pu.draw(cameraPos, 0);
    }
}

void GameManager::drawMapsHelp(int target) {
    C2D_DrawImageAt(menu1Img, 0 + (target * (offset1 * slider)), 0, 0);
    C2D_DrawImageAt(menu2Img, 0 + (target * (offset2 * slider)), 0, 0);
    C2D_DrawImageAt(menu3Img, 0 + (target * (offset3 * slider)), 0, 0);

    C2D_DrawText(&g_staticText[6], C2D_WithColor + C2D_AlignCenter, 200.0f, 30.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));

    C2D_TextBufClear(mapTextBuf);
    int end = std::min(mapScroll + visibleMapCount, static_cast<int>(mapTitles.size()));
    for (int i = mapScroll; i < end; i++)
    {
        C2D_Text text;
        C2D_TextFontParse(&text, customFont, mapTextBuf, mapTitles[i].c_str());
        C2D_TextOptimize(&text);
        u32 color = (i == currentMapInd) ? C2D_Color32(255, 255, 0, 255) : C2D_Color32(255, 255, 255, 255);
        C2D_DrawText(&text, C2D_WithColor + C2D_AlignCenter, 200.0f, 60.0f + (i - mapScroll) * 28.0f, 1.0f, 0.5f, 0.5f, color);

        if (i == mapSelect)
            C2D_DrawText(&g_staticText[2], C2D_WithColor, 100.0f, 60.0f + (i - mapScroll) * 28.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
    }
}

void GameManager::drawTitleHelp(int target) {
    C2D_DrawImageAt(menu1Img, 0 + (target * (offset1 * slider)), 0, 0);
    C2D_DrawImageAt(menu2Img, 0 + (target * (offset2 * slider)), 0, 0);
    C2D_DrawImageAt(menu3Img, 0 + (target * (offset3 * slider)), 0, 0);

    C2D_DrawText(&g_staticText[0], C2D_WithColor + C2D_AlignCenter, 200.0f, 150.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
    C2D_DrawText(&g_staticText[6], C2D_WithColor + C2D_AlignCenter, 200.0f, 180.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
    C2D_DrawText(&g_staticText[1], C2D_WithColor + C2D_AlignCenter, 200.0f, 210.0f, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
    C2D_DrawText(&g_staticText[2], C2D_WithColor, 100.0f, 150.0f + (menuSelect - 1) * 30, 1.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
    C2D_DrawImageAt(logoImg, 75, 10, 1);
}

void GameManager::drawGameOverHelp(int target) {
    C2D_DrawImageAt(gameOver1Img, 0 + (target * (offset1 * slider)), 0, 0);
    C2D_DrawImageAt(gameOver2Img, 0 + (target * (offset2 * slider)), 0, 0);
    C2D_DrawImageAt(gameOver3Img, 0 + (target * (offset3 * slider)), 0, 0);
}