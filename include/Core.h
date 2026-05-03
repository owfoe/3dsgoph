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
}

namespace Const
{
    constexpr float SCREEN_WIDTH = 400.0f;
    constexpr float SCREEN_HEIGHT = 240.0f;
    constexpr float GROUND_HEIGHT = 64.0f;
    constexpr float RC_DIFF_H = 3.0f;
}
