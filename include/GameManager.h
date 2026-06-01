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
#include "Pointer.h"
#include "Powerup.h"

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

public:
    C3D_RenderTarget *topRight, *botLeft;
    Camera camera;
    touchPosition touch;
    Pointer pointer;

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
    void powerupCollisions(Powerup &pu);
    void resolveX(Entity &entity, Ground &ground);
    void resolveY(Entity &entity, Ground &ground);
    bool isHorizontalCollisionPrimary(Entity &entity, Ground &ground);

    void updateTimer() { timer++; }
};
