#include "Player.h"
#include "Ground.h"

Player::Player(float x, float y, float height, float width)
    : Entity(x, y, height, width, PlayerSettings::hp, PlayerSettings::speed), raycast(height, width) {}

void Player::jump()
{
    if (onGround)
    {
        if (std::abs(vx) != speed)
            vy = 7.5f;
        else
            vy = 9.5f;
    }
    isJump = true;
    onGround = false;
}
void Player::updateJump(bool jumpButtonDown)
{
    if (isJump && !onGround)
    {
        if (jumpButtonDown && vy > 0.0f)
            vy -= gravityLow;
        else
            vy -= gravityHigh;
    }
}
void Player::updateFall()
{
    if (!onGround)
    {
        if (vy < -maxFallSpeed)
            vy = -maxFallSpeed;
    }
}
void Player::updateFallOnGround(Ground &ground)
{
    onGround = true;
    isJump = false;
    vy = 0;
    y = ground.getY() - height;
}
void Player::update(bool jumpButtonDown)
{
    updateJump(jumpButtonDown);
    updateFall();
    y -= vy;
}
void Player::draw() { C2D_DrawRectSolid(x, y, 0, width, height, C2D_Color32f(1, 0, 0, 1)); }
void Player::moveLeft()
{
    Entity::moveLeft();
    vx = -speed;
}
void Player::moveRight()
{
    Entity::moveRight();
    vx = speed;
}