#pragma once
#include "Enemy.h"
#include "FallLogic.h"

class GroundEnemy : public Enemy
{
public:
    GroundEnemy(float x, float y, float height, float width, int hp, float speed, uint64_t cooldown,
                char type, float aggrRadius, float attackRadius, char patrolType, float patrolRadius = 0.0f);

    FallLogic fallLogic;
    void update(std::vector<Projectile> &projectiles, float playerCentreX, float playerCentreY, uint64_t timer) override;
    void draw(float cameraPos) override;
    void landOnGround(Ground *ground) override
    {
        Entity::landOnGround(ground);
        fallLogic.setIsFall(false);
    }
    void patrol() override;
    void aggr(float playerCentreX, float playerCentreY, float distToPlayer) override;
    void attack(std::vector<Projectile> &projectiles, float objCentreX, float objCentreY, uint64_t timer) override;
};