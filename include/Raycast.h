#pragma once
#include "HitBox.h"
#include "Core.h"

class Raycast
{
public:
    Raycast() {}
    Raycast(float height, float width) : hitbox(height, 0.0f, Const::RC_DIFF_H, width) {}
    HitBox hitbox;
};