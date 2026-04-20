#pragma once
#include "HitBox.h"
#include "Constants.h"

class Raycast
{
private:
    float x;
    float y;

public:
    Raycast() {}
    Raycast(float height, float width) : hitbox(height - Const::RC_diff_h, 0, Const::RC_diff_h, width) {}
    HitBox hitbox;
};