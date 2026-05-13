#pragma once
#include <vector>
#include <memory>
#include "BaseObject.h"
#include "Player.h"
#include "Ground.h"
#include "Camera.h"
#include "GroundEnemy.h"
#include <3ds.h>

class GameManager
{
private:
    int a;
    std::vector<std::unique_ptr<BaseObject>> objects;
    std::vector<Ground> grounds;
    std::vector<GroundEnemy> groundEnemies;
    Player player;

public:
    C3D_RenderTarget *topRight, *botLeft;
	Camera camera;
    std::vector<std::unique_ptr<BaseObject>> &get_objects() { return objects; }
    std::vector<Ground> &get_grounds() { return grounds; }

    GameManager(int state);
    void init();
    void exit();
    void update(int &s);
    void draw();
    long long getTime();

    void CollisionsManager();
    void EntityGroundCollisions(Entity &entity);

    void resolveX(Entity &entity, Ground &ground);

    void resolveY(Entity &entity, Ground &ground);

    bool isHorizontalCollisionPrimary(Entity &entity, Ground &ground);
};
