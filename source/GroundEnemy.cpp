#include "GroundEnemy.h"
#include "Logger.h"

GroundEnemy::GroundEnemy(float x, float y, float height, float width, int hp, float speed,
                         char type, float aggrRadius, float attackRadius, char patrolType, float patrolRadius)
    : Enemy(x, y, height, width, hp, speed, type, aggrRadius, attackRadius, patrolType, patrolRadius) {}

void GroundEnemy::update()
{
    bool isOnGround = (groundPlatform == nullptr) ? false : true;
    vy = fallLogic.updateFall(isOnGround, false, vy);

    switch (state)
    {
    case State::Patrol:
        if (vx == 0)
            vx = -(speed * view);
        patrol();
        break;

    default:
        break;
    }
}
void GroundEnemy::patrol()
{
    switch (patrolType)
    {
    case 'F':
        break;
    case 'W':
        if (lastHitX == 1)
            moveLeft();
        else if (lastHitX == -1)
            moveRight();
        break;
    case 'R':
        if (x >= spawnX + patrolRadius)
            moveLeft();
        else if (x <= spawnX - patrolRadius)
            moveRight();

    default:
        break;
    }
}

void GroundEnemy::draw() { C2D_DrawRectSolid(x, y, 1, width, height, C2D_Color32f(0, 0, 1, 1)); }