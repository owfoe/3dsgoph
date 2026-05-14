#include "GroundEnemy.h"
#include "Logger.h"

GroundEnemy::GroundEnemy(float x, float y, float height, float width, int hp, float speed, uint64_t cooldown,
                         char type, float aggrRadius, float attackRadius, char patrolType, float patrolRadius)
    : Enemy(x, y, height, width, hp, speed, cooldown, type, aggrRadius, attackRadius, patrolType, patrolRadius) {}

void GroundEnemy::update(float playerCentreX, float playerCentreY, uint64_t timer)
{
    Enemy::update(playerCentreX, playerCentreY, timer);
    bool isOnGround = (groundPlatform == nullptr) ? false : true;
    vy = fallLogic.updateFall(isOnGround, false, vy);

    float dist = distToObj(playerCentreX, playerCentreY);
    switch (state)
    {
    case State::Patrol:
        // Logger::info("Patrol");
        patrol();
        break;
    case State::Aggr:
        // Logger::info("Aggr");
        aggr(playerCentreX, playerCentreY, dist);
        break;
    case State::Attack:
        // Logger::info("Attack");
        attack(playerCentreX, playerCentreY, timer);
        break;

    default:
        break;
    }
}
void GroundEnemy::patrol()
{
    if (vx == 0)
        vx = -(speed * view);
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

void GroundEnemy::aggr(float playerCentreX, float playerCentreY, float distToPlayer)
{
    Enemy::aggr(playerCentreX, playerCentreY, distToPlayer);
    if (distToPlayer >= attackRadius)
    {
        if (view == -1.0f)
            moveLeft();
        else
            moveRight();
    }
}

void GroundEnemy::attack(float objCentreX, float objCentreY, uint64_t timer)
{
    Entity::attack(timer);
    float projectileSpawnX = (view == 1) ? x + width : x;
    float projectileSpawnY = y + height / 4;
    projectiles.push_back(Projectile(projectileSpawnX, projectileSpawnY, 5.0f, 5.0f, 6.0f, timer, 'A', objCentreX, objCentreY));
}

void GroundEnemy::draw(float cameraPos) { C2D_DrawRectSolid(x - cameraPos, y, 1, width, height, C2D_Color32f(0, 0, 1, 1)); }
