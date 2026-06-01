#pragma once
#include "BaseObject.h"

class Sword : public BaseObject
{
private:
    char owner;
    int power;
    int view = 0;

    float angle = -90.0f;
    float endAngle = 90.0f;
    float angleSpeed = 12.0f;

    float pivotX;
    float pivotY;

public:
    Sword() {}
    Sword(float x, float y, float height, float width, char owner, float pivotX, float pivotY, int view, int power)
        : BaseObject(x, y, height, width, 0.0f), owner(owner), power(power), view(view), pivotX(pivotX), pivotY(pivotY) {}

    void draw(float cameraPos, int layer) override { C2D_DrawRectSolid(x - cameraPos, y, 1, width, height, C2D_Color32f(255, 255, 0, 1)); }
    void update() override
    {
        angle += angleSpeed;

        if (angle >= endAngle)
        {
            angle = endAngle;
            isDead = true;
        }

        float rad = angle * 3.14159265f / 180.0f;

        x = pivotX + std::cos(rad) * height;
        y = pivotY + std::sin(rad) * height;
    }
};