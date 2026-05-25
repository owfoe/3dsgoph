#pragma once
#include <string>

namespace ProjectSettings
{
    const std::string NAME = "3dsGAME";
    constexpr bool DEBUG = true;
    constexpr bool CONSOLE = true;
}

namespace PlayerSettings
{
    constexpr int HP = 3;
    constexpr float SPEED = 5.0f;
    constexpr int COOLDOWN = 60;
}

namespace WorldSettings
{
    constexpr float GRAVITY = 0.5f;
    constexpr float MAX_FALL_SPEED = 10.0f;
}

namespace Const
{
    constexpr float SCREEN_WIDTH = 400.0f;
    constexpr float SCREEN_HEIGHT = 240.0f;
    constexpr float RC_DIFF_H = 3.0f;
    constexpr int TIMER = 60 * 60;

    constexpr float POWERUP_SPEED = 0.4f;
    constexpr float POWERUP_RADIUS = 5.0f;
    constexpr float POWERUP_SIZE = 30.0f;
}
