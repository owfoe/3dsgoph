#pragma once
#include "Entity.h"

class Enemy : public Entity
{
protected:
    EnemyPatrolType patrolType;
    float patrolRadius;
    float aggrRadius;
    float attackRadius;
    EnemyState state = EnemyState::Patrol;

public:
    Enemy(float x, float y, float height, float width, int hp, float speed, uint64_t cooldown,
          AttackType attackType, float aggrRadius, float attackRadius, EnemyPatrolType patrolType, float patrolRadius)
        : Entity(x, y, height, width, hp, speed, cooldown, attackType), patrolType(patrolType),
          patrolRadius(patrolRadius), aggrRadius(aggrRadius), attackRadius(attackRadius) {}

    virtual void patrol();
    virtual void aggr(float playerCentreX, float playerCentreY, float distToPlayer)
    {
        viewToPlayer(playerCentreX);
    }

    void viewToPlayer(float playerCentreX)
    {
        view = (playerCentreX >= getCentreX()) ? 1 : -1;
    }

    void moveLeft() override
    {
        Entity::moveLeft();
        view = -1;
    }
    void moveRight() override
    {
        Entity::moveRight();
        view = 1;
    }
    using BaseObject::update;
    virtual void update(std::vector<Projectile> &projectiles, float playerCentreX, float playerCentreY, uint64_t timer)
    {
        checkDeath();
        float dist = distToObj(playerCentreX, playerCentreY);
        if (dist <= attackRadius && state == EnemyState::Aggr && timer > lastAttack + cooldown)
        {
            state = EnemyState::Attack;
        }
        else if (dist <= aggrRadius)
            state = EnemyState::Aggr;
        else
            state = EnemyState::Patrol;
    }
    uint64_t getAttackStartup(AttackType type) override
    {
        switch (type)
        {
        case AttackType::Sword:
            return 24;
        case AttackType::Shot:
            return 12;
        default:
            return 0;
        }
    }
};