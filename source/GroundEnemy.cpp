#include "GroundEnemy.h"

GroundEnemy::GroundEnemy(float x, float y, float height, float width, int hp, float speed,
                         char type, float aggrRadius, float attackRadius, char potrolType, float potrolRadius)
    : Enemy(x, y, height, width, hp, speed, type, aggrRadius, attackRadius, potrolType, potrolRadius) {}

void GroundEnemy::update()
{
    bool isOnGround = (groundPlatform == nullptr) ? false : true;
    vy = fallLogic.updateFall(isOnGround, false, vy);
}

void GroundEnemy::draw() { C2D_DrawRectSolid(x, y, 1, width, height, C2D_Color32f(0, 0, 1, 1)); }