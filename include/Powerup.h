#pragma once
#include "BaseObject.h"

class Powerup : public BaseObject
{
private:
    bool isPickedUp = false;
    bool isUsing = false;
    uint64_t duration;
    uint64_t startUsing;

public:
    Powerup() {}
    Powerup(float x, float y, float height, float width, uint64_t duration)
        : BaseObject(x, y, height, width, Const::POWERUP_SPEED), duration(duration) { vy = -speed; }

    bool getIsPickedUp() { return isPickedUp; }
    bool getIsUsing() { return isUsing; }

    void setIsPickedUp(bool flag) { isPickedUp = flag; }
    void setIsUsing(bool flag) { isUsing = flag; }

    void pickUp() { isPickedUp = true; }
    void use(uint64_t timer)
    {
        startUsing = timer;
        isUsing = true;
    }
    void checkDeath(uint64_t timer) { isDead = (startUsing + duration >= timer) ? true : false; }
    void draw(float cameraPos, int layer) override
    {
        C2D_DrawRectSolid(x - cameraPos, y, layer, width, height, C2D_Color32f(128, 0, 128, 1.0));
    }
    using BaseObject::update;
    void update(uint64_t timer)
    {
        float omega = Const::POWERUP_SPEED / Const::POWERUP_RADIUS;
        y = spawnY + Const::POWERUP_RADIUS * std::sinf(timer * omega);

        // if (y >= spawnY + Const::POWERUP_RADIUS || y <= spawnY - Const::POWERUP_RADIUS)
        //     vy *= -1.0f;
        // y -= vy;
    }
};