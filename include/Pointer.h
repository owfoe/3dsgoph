#pragma once
#include "BaseObject.h"
class Pointer : public BaseObject
{
public:
    Pointer() {}
    Pointer(float x, float y, float height, float width)
        : BaseObject(x, y, height, width, 0.0f) {}

    void draw(float cameraPos, int layer) {}
};