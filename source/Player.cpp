#include "Player.h"
#include "Ground.h"

Player::Player(float x, float y, float height, float width)
    : Entity(x, y, height, width, PlayerSettings::HP, PlayerSettings::SPEED) {}

void Player::jump()
{
    if (groundPlatform != nullptr)
    {
        if (std::abs(vx) != speed)
            vy = 7.5f;
        else
            vy = 9.5f;
    }
    isJump = true;
    // onGround = false;
    fallLogic.setIsFall(false);
    groundPlatform = nullptr;
}

void Player::updateJump(bool jumpButtonDown)
{
    if (isJump && groundPlatform == nullptr)
    {
        if (jumpButtonDown && vy > 0.0f)
            vy -= jumpGravityLow;
        else
            vy -= jumpGravityHigh;
    }
}

void Player::update(bool jumpButtonDown)
{
    updateJump(jumpButtonDown);
    bool isOnGround = (groundPlatform == nullptr) ? false : true;
    vy = fallLogic.updateFall(isOnGround, isJump, vy);
}

void Player::draw() { C2D_DrawRectSolid(x, y, 1, width, height, C2D_Color32f(1, 0, 0, 1)); }
