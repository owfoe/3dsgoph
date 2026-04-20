#pragma once
#include "BaseObject.h"
#include <cmath>

class Ground : public BaseObject
{
private:
    int mode;
    bool isBarrier;
    float speed;
    float vx = 0.0f;
    float vy = 0.0f;

    float delta = 0.5f;
    float radius = 0.0f;
    float centreX;
    float centreY;

public:
    Ground(float x, float y, float height, float width, bool isBarrier)
        : BaseObject(x, y, height, width), mode(0), isBarrier(isBarrier), speed(0.0f), centreX(x), centreY(y) {}
    Ground(float x, float y, float height, float width, bool isBarrier, int mode, float speed, float radius)
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
        case 4:
            vx = -speed;
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
            if (x >= centreX + radius || x <= centreX - radius)
                vx *= -1.0f;
            break;
        case 2:
            if (y >= centreY + radius || y <= centreY - radius)
                vy *= -1.0f;
        case 3:
        {
            float radiusXY = std::sqrt(std::pow(radius, 2) / 2);
            if (x >= centreX + radiusXY || x <= centreX - radiusXY)
                vx *= -1.0f;
            if (y >= centreY + radiusXY || y <= centreY - radiusXY)
                vy *= -1.0f;
            break;
        }
        case 4:
        {
            float radiusXY = std::sqrt(std::pow(radius, 2) / 2);
            if (x >= centreX + radiusXY || x <= centreX - radiusXY)
                vx *= -1.0f;
            if (y >= centreY + radiusXY || y <= centreY - radiusXY)
                vy *= -1.0f;
            break;
        }
        default:
            break;
        }
        x += vx;
        y -= vy;
    }
    bool getIsBarrier() { return isBarrier; }
};