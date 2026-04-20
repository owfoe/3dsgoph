#pragma once
#include "BaseObject.h"

class Projectile : public BaseObject
{
public:
    Projectile(float x, float y, float height, float width)
        : BaseObject(x, y, height, width) {}

    void update() {}
};