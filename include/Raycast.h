#pragma once
#include "HitBox.h"
#include "Constants.h"

class Raycast
{
public:
    Raycast() {}
    Raycast(float height, float width) : hitbox(height, 0.0f, Const::RC_diff_h, width) {}
    HitBox hitbox;
};