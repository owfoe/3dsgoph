#pragma once
#include "Entity.h"
#include "Raycast.h"

class Player : public Entity
{
public:
    Player(float x, float y, float height, float width, int hp, float speed);

    Raycast raycast;

    void jump();
    void updateJump();
};
