#pragma once
#include "Entity.h"
#include "Raycast.h"

class Player : public Entity
{
public:
    Player() {}
    Player(float x, float y, float height, float width, int hp, float speed);

    Raycast raycast;

    void jump();
    void updateJump();
    void draw() override
    {
        C2D_DrawRectSolid(x, y, 0, width, height, C2D_Color32f(1, 0, 0, 1));
    }
};
