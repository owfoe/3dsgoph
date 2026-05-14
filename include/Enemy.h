#pragma once
#include "Entity.h"

class Enemy : public Entity
{
protected:
    // M - melee, R - ranged
    char type;

    // Ground: F - fixed, W - wall to wall, R - radius
    // Fly: F - fixed, H - horizontal, V - vertical, C - circle shape, I - infinity shape
    char patrolType;
    float patrolRadius;
    float aggrRadius;
    float attackRadius;
    enum class State
    {
        Patrol,
        Aggr,
        Attack
    };
    State state = State::Patrol;

public:
    Enemy(float x, float y, float height, float width, int hp, float speed, uint64_t cooldown,
          char type, float aggrRadius, float attackRadius, char patrolType, float patrolRadius)
        : Entity(x, y, height, width, hp, speed, cooldown), type(type), patrolType(patrolType),
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
    virtual void update(float playerCentreX, float playerCentreY, uint64_t timer)
    {
        float dist = distToObj(playerCentreX, playerCentreY);
        // Logger::log(timer, lastAttack, cooldown, timer - lastAttack > cooldown);
        if (dist <= attackRadius && state == State::Aggr && timer - lastAttack > cooldown)
            state = State::Attack;
        else if (dist <= aggrRadius)
            state = State::Aggr;
        else
            state = State::Patrol;
    }
    using Entity::attack;
    virtual void attack(float objCentreX, float objCentreY, uint64_t timer);
};