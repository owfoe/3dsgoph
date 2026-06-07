#include "FlyEnemy.h"
#include <algorithm>

FlyEnemy::FlyEnemy(float x, float y, float height, float width, int hp, float speed, uint64_t cooldown,
                   AttackType attackType, float aggrRadius, float attackRadius, EnemyPatrolType patrolType, float patrolRadius)
    : Enemy(x, y, height, width, hp, speed, cooldown, attackType, aggrRadius, attackRadius, patrolType, patrolRadius) {}

void FlyEnemy::update(std::vector<Projectile> &projectiles, float playerCentreX, float playerCentreY, uint64_t timer)
{
    Enemy::update(projectiles, playerCentreX, playerCentreY, timer);

    switch (state)
    {
    case EnemyState::Patrol:
        patrol();
        break;
    case EnemyState::Aggr:
    {
        float dist = distToObj(playerCentreX, playerCentreY);
        aggr(playerCentreX, playerCentreY, dist);
        break;
    }
    case EnemyState::Attack:
        setNullVX();
        setNullVY();
        startAttack(attackType, timer, playerCentreX, playerCentreY);
        break;

    default:
        break;
    }

    // if (state != EnemyState::Patrol || (patrolType != EnemyPatrolType::CircleShape && patrolType != EnemyPatrolType::InfinityShape))
    // {
    //     changeX(vx);
    //     changeY(-vy);
    // }
}

void FlyEnemy::patrol()
{
    switch (patrolType)
    {
    case EnemyPatrolType::Fixed:
        break;
    case EnemyPatrolType::Horizontal:
        if (std::clamp(y, spawnY - Const::ENEMY_SPAWN_AREA_RADIUS, spawnY + Const::ENEMY_SPAWN_AREA_RADIUS) != y)
            goToSpawn();
        if (vx == 0.0f)
            vx = speed * view;
        if (x >= spawnX + patrolRadius)
            moveLeft();
        else if (x <= spawnX - patrolRadius)
            moveRight();
        break;
    case EnemyPatrolType::Vertical:
        if (std::clamp(x, spawnX - Const::ENEMY_SPAWN_AREA_RADIUS, spawnX + Const::ENEMY_SPAWN_AREA_RADIUS) != x)
            goToSpawn();
        if (vy == 0.0f)
            vy = speed * viewY;
        if (y <= spawnY - patrolRadius)
            moveDown();
        else if (y >= spawnY + patrolRadius)
            moveUp();
        break;

    default:
        break;
    }
}

void FlyEnemy::aggr(float playerCentreX, float playerCentreY, float distToPlayer)
{
    Enemy::aggr(playerCentreX, playerCentreY, distToPlayer);
    if (distToPlayer <= attackRadius)
        return;

    float dx = getDXtoObj(playerCentreX);
    float dy = getDYtoObj(playerCentreY);
    vx = dx / distToPlayer * speed;
    vy = -dy / distToPlayer * speed;
}

void FlyEnemy::goToSpawn()
{
    float len = distToObj(spawnX + width / 2.0f, spawnY + height / 2.0f);
    if (len == 0.0f)
        return;
    float dx = getDXtoObj(spawnX + width / 2.0f);
    float dy = getDYtoObj(spawnY + height / 2.0f);
    vx = dx / len * speed;
    vy = -dy / len * speed;
}

void FlyEnemy::draw(float cameraPos, int layer)
{
    C2D_DrawRectSolid(x - cameraPos, y, layer, width, height, C2D_Color32f(0, 1, 1, 1));
}
