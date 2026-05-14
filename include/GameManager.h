#pragma once
#include <vector>
#include <memory>
#include <3ds.h>
#include "BaseObject.h"
#include "Player.h"
#include "Ground.h"
#include "GroundEnemy.h"
#include "Core.h"

class GameManager
{
private:
    int a;
    uint64_t timer = 0;
    std::vector<std::unique_ptr<BaseObject>> objects;
    std::vector<Ground> grounds;
    std::vector<GroundEnemy> groundEnemies;
    Player player;

public:
    C3D_RenderTarget *topRight, *botLeft;

    std::vector<std::unique_ptr<BaseObject>> &get_objects() { return objects; }
    std::vector<Ground> &get_grounds() { return grounds; }

    GameManager(int state);
    void init();
    void exit();
    void update(int &s);
    void draw();
    long long getTime();

    void collisionsManager();
    void eraseManager();
    void entityGroundCollisions(Entity &entity);
    void projectileCollisions(Projectile &p, char from);
    void resolveX(Entity &entity, Ground &ground);
    void resolveY(Entity &entity, Ground &ground);
    bool isHorizontalCollisionPrimary(Entity &entity, Ground &ground);

    void updateTimer() { timer++; }
};
