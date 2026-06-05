#pragma once
#include "BaseObject.h"
#include <cmath>

class Ground : public BaseObject
{
private:
    GroundMode mode;
    bool isBarrier;

    float radius = 0.0f;

public:
    Ground(float x, float y, float height, float width, bool isBarrier, GroundMode mode)
        : BaseObject(x, y, height, width), mode(mode), isBarrier(isBarrier) {}
    Ground(float x, float y, float height, float width, bool isBarrier, GroundMode mode, float radius, float speed = 0.0f)
        : BaseObject(x, y, height, width, speed), mode(mode), isBarrier(isBarrier), radius(radius)
    {
        switch (mode)
        {
        case GroundMode::Horizontal:
            vx = speed;
            break;
        case GroundMode::Vertical:
            vy = -speed;
            break;
        case GroundMode::Descent:
            vx = speed;
            vy = -speed;
            break;
        case GroundMode::Rise:
            vx = -speed;
            vy = -speed;
            break;
        default:
            break;
        }
    }

    void moveLeft() { changeX(-getSpeed()); }
    void moveRight() { changeX(getSpeed()); }

    void draw(float cameraPos, int layer) override
    {
        C2D_DrawRectSolid(x - cameraPos, y, layer, width, height, C2D_Color32f(0, 1, 0, 1));
    }

    using BaseObject::update;
    void update(uint64_t timer)
    {
        switch (mode)
        {
        case GroundMode::Horizontal:
            // x = spawnX + radius * std::sinf(timer * omega);
            // vx = x - prevX;
            if (x >= spawnX + radius || x <= spawnX - radius)
                vx *= -1.0f;
            break;
        case GroundMode::Vertical:
            // y = spawnY + radius * std::cosf(timer * omega);
            // vy = y - prevY;
            if (y >= spawnY + radius || y <= spawnY - radius)
                vy *= -1.0f;
            break;
        case GroundMode::Descent:
        {
            float radiusXY = std::sqrt(std::pow(radius, 2) / 2);
            if (x >= spawnX + radiusXY || x <= spawnX - radiusXY)
                vx *= -1.0f;
            if (y >= spawnY + radiusXY || y <= spawnY - radiusXY)
                vy *= -1.0f;
            break;
        }
        case GroundMode::Rise:
        {
            float radiusXY = std::sqrt(std::pow(radius, 2) / 2);
            if (x >= spawnX + radiusXY || x <= spawnX - radiusXY)
                vx *= -1.0f;
            if (y >= spawnY + radiusXY || y <= spawnY - radiusXY)
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