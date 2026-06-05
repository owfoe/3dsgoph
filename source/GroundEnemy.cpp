#include "GroundEnemy.h"
#include "Logger.h"

GroundEnemy::GroundEnemy(float x, float y, float height, float width, int hp, float speed, uint64_t cooldown, AttackType attackType, float aggrRadius, float attackRadius, EnemyPatrolType patrolType, float patrolRadius)
    : Enemy(x, y, height, width, hp, speed, cooldown, attackType, aggrRadius, attackRadius, patrolType, patrolRadius) {}

void GroundEnemy::update(std::vector<Projectile> &projectiles, float playerCentreX, float playerCentreY, uint64_t timer)
{
    Enemy::update(projectiles, playerCentreX, playerCentreY, timer);
    bool isOnGround = (groundPlatform == nullptr) ? false : true;
    vy = fallLogic.updateFall(isOnGround, false, vy);

    switch (state)
    {
    case EnemyState::Patrol:
        // Logger::info("Patrol");
        patrol();
        break;
    case EnemyState::Aggr:
        // Logger::info("Aggr");
        {
            float dist = distToObj(playerCentreX, playerCentreY);
            aggr(playerCentreX, playerCentreY, dist);
            break;
        }
    case EnemyState::Attack:
        startAttack(attackType, timer, playerCentreX, playerCentreY);
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
    case EnemyPatrolType::Fixed:
        break;
    case EnemyPatrolType::WallToWall:
        if (lastHitX == 1)
            moveLeft();
        else if (lastHitX == -1)
            moveRight();
        break;
    case EnemyPatrolType::Radius:
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
    if (distToPlayer > attackRadius)
    {
        if (view == -1.0f)
            moveLeft();
        else
            moveRight();
    }
}

void GroundEnemy::draw(float cameraPos, int layer) { C2D_DrawRectSolid(x - cameraPos, y, layer, width, height, C2D_Color32f(0, 0, 1, 1)); }
