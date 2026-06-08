#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <3ds.h>
#include "BaseObject.h"
#include "Player.h"
#include "Ground.h"
#include "Camera.h"
#include "GroundEnemy.h"
#include "FlyEnemy.h"
#include "Core.h"
#include "LowerScreen.h"
#include "Pointer.h"
#include "Powerup.h"
#include "Marker.h"
#include "MapLoader.h"
#include "MicrophoneInput.h"

class GameManager
{
private:
    GameManagerState state;
    uint64_t timer = 0;
    std::vector<Ground> grounds;
    std::vector<GroundEnemy> groundEnemies;
    std::vector<FlyEnemy> flyEnemies;
    std::vector<Projectile> projectiles;
    std::vector<Powerup> powerups;
    Player player;
    std::unordered_map<std::string, MapData> maps;
    std::string currentMap;
    int currentMapInd;
    std::vector<std::string> mapTitles;
    float mapWidth = Const::SCREEN_WIDTH;
    C2D_TextBuf mapTextBuf;
    int mapSelect = 0, mapScroll = 0, mapCount, visibleMapCount = ProjectSettings::VISIBLE_MAP_COUNT;

    float nearestEnemyX;
    float nearestEnemyY;

    u32 kDown, kHeld;
    long long switchTimer;
    GameManagerState nextState;
    bool isJumpButtonDown;
    int offset1, offset2, offset3, playerHp, target, powerupSize, menuSelect = 1,
    maxSelect = ProjectSettings::MENUS_COUNT;
    float cameraPos, playerPos, dx, cameraSpeed, slider;
    C3D_RenderTarget *topRight, *topLeft, *botLeft;
    C2D_SpriteSheet hpSheet, gameOver1Sheet, gameOver2Sheet, gameOver3Sheet, logoSheet, menu1Sheet,
    menu2Sheet, menu3Sheet, lowerMenuSheet;
    C2D_Image heartImg, gameOver1Img, gameOver2Img, gameOver3Img, logoImg, menu1Img, menu2Img,
    menu3Img, lowerMenuImg;
    C2D_TextBuf g_staticBuf;
    C2D_Font customFont;
    C2D_Text g_staticText[10];
    Camera camera;
    touchPosition touch;
    Pointer pointer;
    LowerScreen ls;
    Marker marker;

    MicrophoneInput microphone;
    bool microphoneReady = false;

public:
    GameManager(int s);
    void init();
    void textInit();
    void exit();
    void update(int &s);
    void loadUpdate(int& s);
    void titleUpdate(int& s);
    void mapsUpdate();
    void gameUpdate();
    void draw();
    void loadDraw();
    void titleDraw();
    void mapsDraw();
    void gameOverDraw();
    void gameDraw();
    long long getTime() { return timer; }

    void collisionsManager();
    void eraseManager();
    void entityGroundCollisions(Entity &entity);
    void projectileCollisions(Projectile &p);
    void powerupCollisions(Powerup *pu);
    void resolveX(Entity &entity, Ground &ground);
    void resolveY(Entity &entity, Ground &ground);
    bool isHorizontalCollisionPrimary(Entity &entity, Ground &ground);

    void attackManager();
    void resolveAttack(Entity &attacker, OwnerType owner, PendingAttack attack);
    void resolveSwordAttack(Entity &attacker, OwnerType owner, int view);
    bool isInSwordArc(Entity &attacker, Entity &target, int view);

    void updateTimer() { timer++; }
    GameManagerState getState() { return state; }
    void setState(GameManagerState s) { this->state = s; }
    void updateCamera();
    void loadSwitch(GameManagerState previousState);

    void loadMap(std::string fileName);
    void createMap();
    std::vector<std::string> getFiles(const std::string &folder);

    void drawGameHelp();
    void drawMapsHelp(int target);
    void drawTitleHelp(int target);
    void drawGameOverHelp(int target);
};
