#pragma once
#include "Core.h"
#include "Logger.h"

class FallLogic
{
private:
    bool isFall = true;

public:
    bool getIsFall() { return isFall; }
    void setIsFall(bool flag) { isFall = flag; }
    float updateFall(bool isOnGround, bool isJump, float vy)
    {
        if (!isOnGround)
        {
            if (!isJump)
            {
                vy -= WorldSettings::GRAVITY;
            }
            if (vy < -WorldSettings::MAX_FALL_SPEED)
                vy = -WorldSettings::MAX_FALL_SPEED;
        }
        if (vy <= 0.0f)
        {
            isFall = true;
        }
        return vy;
    }
};