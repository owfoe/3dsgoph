#pragma once
#include "BaseObject.h"

class Ground : public BaseObject
{
private:
    int mode;
    bool isBarrier;
    float speed;
    float vx = 0.0f;
    float vy = 0.0f;

    float delta = 0.5f;
    float radius;
    float centreX;
    float centreY;

public:
    Ground(float x, float y, float height, float width, bool isBarrier)
        : BaseObject(x, y, height, width), mode(0), isBarrier(isBarrier), speed(0.0f) {}
    Ground(float x, float y, float height, float width, bool isBarrier, bool mode, float speed, float radius)
        : BaseObject(x, y, height, width), mode(mode), isBarrier(isBarrier), speed(speed), radius(radius), centreX(x), centreY(y)
    {
        switch (mode)
        {
        case 1:
            vx = speed;
            break;
        case 2:
            vy = -speed;
            break;
        case 3:
            vx = speed;
            vy = -speed;
            break;

        default:
            break;
        }
    }

    int getMode() const { return mode; }
    float getSpeed() const { return speed; }
    void moveLeft() { changeX(-getSpeed()); }
    void moveRight() { changeX(getSpeed()); }
    void draw() override
    {
        C2D_DrawRectSolid(x, y, 0, width, height, C2D_Color32f(0, 1, 0, 1));
    }
    void update() override
    {
        switch (mode)
        {
        case 1:
            if (vx > 0.0f && vx <= speed)
                vx -= delta;

            break;

        default:
            break;
        }
    }
    bool getIsBarrier() { return isBarrier; }
};