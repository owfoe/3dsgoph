#pragma once
#include "Enemy.h"
#include "FallLogic.h"

class GroundEnemy : public Enemy
{
public:
    GroundEnemy(float x, float y, float height, float width, int hp, float speed,
                char type, float aggrRadius, float attackRadius, char patrolType, float patrolRadius = 0.0f);

    FallLogic fallLogic;
    void draw() override;
    void update() override;
    void landOnGround(Ground *ground) override
    {
        Entity::landOnGround(ground);
        fallLogic.setIsFall(false);
    }
    void patrol() override;
};