#pragma once
#include "BaseObject.h"

class Projectile : public BaseObject
{
private:
    // E - enemy, P - player
    char owner;
    // B - bubble, A - accurate shot
    char type;
    float targetX = 0.0f;
    float targetY = 0.0f;
    int power = 0;
    uint64_t spawnTime;
    int view = 0;

public:
    Projectile() {}
    Projectile(float x, float y, float height, float width, float speed, char owner, uint64_t spawnTime, char type, float targetX, float targetY)
        : BaseObject(x, y, height, width, speed), owner(owner), type(type), targetX(targetX), targetY(targetY), spawnTime(spawnTime)
    {
        if (type == 'A')
        {
            float dx = getDXtoObj(targetX);
            float dy = getDYtoObj(targetY);
            float len = distToObj(targetX, targetY);
            if (len == 0.0f)
                return;
            vx = dx / len * speed;
            vy = dy / len * speed;
        }
    }
    Projectile(float x, float y, float height, float width, float speed, char owner, uint64_t spawnTime, char type, int view, int power)
        : BaseObject(x, y, height, width, speed), type(type), power(power), spawnTime(spawnTime), view(view)
    {
        if (type == 'B')
        {
            vx = speed * (power + 1) * view;
        }
    }

    char getOwner() { return owner; }
    void draw(float cameraPos) override { C2D_DrawRectSolid(x - cameraPos, y, 1, width, height, C2D_Color32f(255, 255, 0, 1)); }
    void update() override
    {
        x += vx;
        y += vy;
        switch (type)
        {
        case 'A':
            break;
        case 'B':
        {
            vy -= 0.1f * 1.5f;
        }
        default:
            break;
        }
    }
};
