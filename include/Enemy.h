#pragma once
#include "Entity.h"

class Enemy : public Entity
{
protected:
    // M - melee, R - ranged
    char type;

    // Ground: F - fixed, W - wall to wall, R - radius
    // Fly: F - fixed, H - horizontal, V - vertical, C - circle shape, I - infinity shape
    char potrolType;
    float potrolRadius;
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
    Enemy(float x, float y, float height, float width, int hp, float speed,
          char type, float aggrRadius, float attackRadius, char potrolType, float potrolRadius)
        : Entity(x, y, height, width, hp, speed), type(type), potrolType(potrolType), potrolRadius(potrolRadius), aggrRadius(aggrRadius), attackRadius(attackRadius) {}
};