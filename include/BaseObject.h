#pragma once
#include <citro2d.h>

#include "HitBox.h"

class BaseObject
{
protected:
    float x, y;
    float height, width;

public:
    BaseObject() {}
    BaseObject(float x, float y, float height, float width)
        : x(x), y(y), height(height), width(width), hitbox(height, width) {}
    virtual ~BaseObject() = default;

    HitBox hitbox;

    float getX() const { return this->x; }
    float getY() const { return this->y; }
    void changeX(float value) { this->x += value; }
    void changeY(float value) { this->y += value; }
    void setX(float x) { this->x = x; }
    void setY(float y) { this->y = y; }

    float getHeight() const { return height; }
    float getWidth() const { return width; }

    virtual void draw() = 0;
};