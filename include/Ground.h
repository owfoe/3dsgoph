#pragma once
#include "BaseObject.h"
#include <cmath>

class Ground : public BaseObject
{
private:
    // S - static, H - horizontal, V - vertical, D - descent, R - rise
    char mode;
    bool isBarrier;

    float radius;

public:
    Ground(float x, float y, float height, float width, bool isBarrier, char mode = 'S', float speed = 0.0f, float radius = 0.0f)
        : BaseObject(x, y, height, width, speed), mode(mode), isBarrier(isBarrier), radius(radius)
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

    void moveLeft() { changeX(-getSpeed()); }
    void moveRight() { changeX(getSpeed()); }

    void draw(float cameraPos, int layer) override
    {
        C2D_DrawRectSolid(x - cameraPos, y, layer, width, height, C2D_Color32f(0, 1, 0, 1));
    }

    using BaseObject::update;
    void update(uint64_t timer)
    {
        float omega = speed / radius;
        switch (mode)
        {
        case 'H':
            // if (x >= spawnX + radius || x <= spawnX - radius)
            //     vx *= -1.0f;

            x = spawnX + radius * std::sinf(timer * omega);
            break;
        case 'V':
            // if (y >= spawnY + radius || y <= spawnY - radius)
            //     vy *= -1.0f;
            y = spawnY + radius * std::cosf(timer * omega);
            break;
        case 'D':
        {
            // float radiusXY = std::sqrt(std::pow(radius, 2) / 2);
            // if (x >= spawnX + radiusXY || x <= spawnX - radiusXY)
            //     vx *= -1.0f;
            // if (y >= spawnY + radiusXY || y <= spawnY - radiusXY)
            //     vy *= -1.0f;
            y = spawnY + radius * std::sinf(timer * omega);
            x = spawnX + radius * std::sinf(timer * omega);
            break;
        }
        case 'R':
        {
            // float radiusXY = std::sqrt(std::pow(radius, 2) / 2);
            // if (x >= spawnX + radiusXY || x <= spawnX - radiusXY)
            //     vx *= -1.0f;
            // if (y >= spawnY + radiusXY || y <= spawnY - radiusXY)
            //     vy *= -1.0f;
            y = spawnY + radius * std::sinf(timer * omega);
            x = spawnX - radius * std::sinf(timer * omega);
            break;
        }
        default:
            break;
        }
    }

    bool getIsBarrier() { return isBarrier; }
};