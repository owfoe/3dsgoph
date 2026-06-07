#pragma once
#include "Enemy.h"

class FlyEnemy : public Enemy
{
private:
    int viewY = 1;

public:
    FlyEnemy(float x, float y, float height, float width, int hp, float speed, uint64_t cooldown,
             AttackType attackType, float aggrRadius, float attackRadius, EnemyPatrolType patrolType, float patrolRadius = 0.0f);

    void update(std::vector<Projectile> &projectiles, float playerCentreX, float playerCentreY, uint64_t timer) override;
    void draw(float cameraPos, int layer) override;
    void patrol() override;
    void aggr(float playerCentreX, float playerCentreY, float distToPlayer) override;
    void goToSpawn();
    void moveUp()
    {
        vy = speed;
        viewY = 1;
    }
    void moveDown()
    {
        vy = -speed;
        viewY = -1;
    }
};
