#include "Player.h"
#include "Ground.h"

Player::Player(float x, float y, float height, float width)
    : Entity(x, y, height, width, PlayerSettings::HP, PlayerSettings::SPEED, PlayerSettings::COOLDOWN) {}

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
    checkDeath();
    updateJump(jumpButtonDown);
    bool isOnGround = (groundPlatform == nullptr) ? false : true;
    vy = fallLogic.updateFall(isOnGround, isJump, vy);
}

void Player::attack(std::vector<Projectile> &projectiles, uint64_t timer)
{
    Entity::attack(projectiles, timer);

    float projectileSpawnX = (view == 1) ? x + width : x;
    float projectileSpawnY = y + height / 4;
    projectiles.push_back(Projectile(projectileSpawnX, projectileSpawnY, 10.0f, 10.0f, 1.0f, 'P', timer, 'B', view, 5));

    // float pivotX = (view == 1) ? x + width : x;
    // float pivotY = y + height / 2;
    // swords.push_back(Sword(x, y - 40.0f, 40.0f, 10.0f, 'P', pivotX, pivotY, view, 5));
}

void Player::draw(float cameraPos) { C2D_DrawRectSolid(x - cameraPos, y, 1, width, height, C2D_Color32f(1, 0, 0, 1)); }
