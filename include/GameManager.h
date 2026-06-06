#pragma once
#include <vector>
#include <memory>
#include <3ds.h>
#include "BaseObject.h"
#include "Player.h"
#include "Ground.h"
#include "Camera.h"
#include "GroundEnemy.h"
#include "Core.h"
#include "LowerScreen.h"
#include "Pointer.h"
#include "Powerup.h"
#include "Marker.h"

class GameManager
{
private:
    int a;
    uint64_t timer = 0;
    std::vector<std::unique_ptr<BaseObject>> objects;
    std::vector<Ground> grounds;
    std::vector<GroundEnemy> groundEnemies;
    std::vector<Projectile> projectiles;
    std::vector<Powerup> powerups;
    Player player;

    float nearestEnemyX;
    float nearestEnemyY;

public:
    u32 kDown, kHeld;
    bool isJumpButtonDown;
    int playerHp, powerupSize;
    float cameraPos, playerPos, dx, cameraSpeed;
    C3D_RenderTarget *topRight, *botLeft;
    C2D_SpriteSheet hpsheet;
    C2D_Image heartImg;
    Camera camera;
    touchPosition touch;
    Pointer pointer;
    LowerScreen ls;
    Marker marker;

    GameManager(int state);
    void init();
    void exit();
    void update(int &s);
    void draw();
    long long getTime();

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
};
