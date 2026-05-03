#pragma once
#include "BaseObject.h"
#include <cmath>

class Ground : public BaseObject
{
private:
    // S - static, H - horizontal, V - vertical, D - descent, R - rise
    char mode;
    bool isBarrier;
    float speed;
    float vx = 0.0f;
    float vy = 0.0f;

    float radius = 0.0f;
    float centreX;
    float centreY;

public:
    Ground(float x, float y, float height, float width, bool isBarrier)
        : BaseObject(x, y, height, width), mode('S'), isBarrier(isBarrier), speed(0.0f), centreX(x), centreY(y) {}

    Ground(float x, float y, float height, float width, bool isBarrier, char mode, float speed, float radius)
        : BaseObject(x, y, height, width), mode(mode), isBarrier(isBarrier), speed(speed), radius(radius), centreX(x), centreY(y)
    {
        switch (mode)
        {
        case 'H':
            vx = speed;
            break;
        case 'V':
            vy = -speed;
            break;
        case 'D':
            vx = speed;
            vy = -speed;
            break;
        case 'R':
            vx = -speed;
            vy = -speed;
            break;
        default:
            break;
        }
    }

    char getMode() const { return mode; }
    float getSpeed() const { return speed; }

    float getVX() const { return vx; }
    float getVY() const { return vy; }

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
        case 'H':
            if (x >= centreX + radius || x <= centreX - radius)
                vx *= -1.0f;
            break;
        case 'V':
            if (y >= centreY + radius || y <= centreY - radius)
                vy *= -1.0f;
            break;
        case 'D':
        {
            float radiusXY = std::sqrt(std::pow(radius, 2) / 2);
            if (x >= centreX + radiusXY || x <= centreX - radiusXY)
                vx *= -1.0f;
            if (y >= centreY + radiusXY || y <= centreY - radiusXY)
                vy *= -1.0f;
            break;
        }
        case 'R':
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
