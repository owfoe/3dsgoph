#pragma once
#include "BaseObject.h"

class Projectile : public BaseObject
{
private:
    OwnerType owner;
    AttackType type;
    float targetX = 0.0f;
    float targetY = 0.0f;
    float power = 0.0f;
    uint64_t spawnTime;
    int view = 0;

public:
    Projectile() {}
    Projectile(float x, float y, float height, float width, float speed, OwnerType owner, uint64_t spawnTime, AttackType type, float targetX, float targetY)
        : BaseObject(x, y, height, width, speed), owner(owner), type(type), targetX(targetX), targetY(targetY), spawnTime(spawnTime)
    {
        if (type == AttackType::Shot)
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
    Projectile(float x, float y, float height, float width, float speed, OwnerType owner, uint64_t spawnTime, AttackType type, int view, float power)
        : BaseObject(x, y, height, width, speed), owner(owner), type(type), power(power), spawnTime(spawnTime), view(view)
    {
        if (type == AttackType::Bubble)
        {
            vx = speed * (power + 1) * view;
        }
    }

    OwnerType getOwner() { return owner; }
    void draw(float cameraPos, int layer) override { C2D_DrawRectSolid(x - cameraPos, y, layer, width, height, C2D_Color32f(255, 255, 0, 1)); }
    void update() override
    {
        x += vx;
        y += vy;
        if (type == AttackType::Bubble)
            vy -= 0.1f * 1.5f;
    }
};
