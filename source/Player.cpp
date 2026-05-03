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
    isFall = false;
    groundPlatform = nullptr;
}

void Player::updateJump(bool jumpButtonDown)
{
    if (isJump && groundPlatform == nullptr)
    {
        if (jumpButtonDown && vy > 0.0f)
            vy -= gravityLow;
        else
            vy -= gravityHigh;
    }
}

void Player::updateFall()
{
    if (groundPlatform == nullptr)
    {
        if (!isJump)
            vy -= gravity;
        if (vy < -maxFallSpeed)
            vy = -maxFallSpeed;
    }
    if (vy <= 0.0f)
    {
        isFall = true;
    }
}

void Player::resetGroundState()
{
    // onGround = false;
    groundPlatform = nullptr;
}

void Player::landOnGround(Ground *ground)
{
    // onGround = true;
    isJump = false;
    isFall = false;
    vy = 0.0f;
    y = ground->getY() - height;
    groundPlatform = ground;
}

void Player::update(bool jumpButtonDown)
{
    updateJump(jumpButtonDown);
    updateFall();
}

void Player::handleConflict(float hitX, float hitY)
{
    if (hitX != -1.0f)
    {
        x = hitX;
        vx = 0.0f;
    }
    if (hitY != -1.0f)
    {
        y = hitY;
        vy = 0.0f;
    }
}

void Player::draw() { C2D_DrawRectSolid(x, y, 1, width, height, C2D_Color32f(1, 0, 0, 1)); }

void Player::moveLeft() { vx = -speed; }
void Player::moveRight() { vx = speed; }
