#include "Player.h"
#include "Ground.h"

Player::Player(float x, float y)
    : Entity(x, y, PlayerSettings::HEIGHT, PlayerSettings::WIDTH, PlayerSettings::HP, PlayerSettings::SPEED, PlayerSettings::COOLDOWN, AttackType::Sword) {}

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

void Player::draw(float cameraPos, int layer) { C2D_DrawRectSolid(x - cameraPos, y, layer, width, height, C2D_Color32f(1, 0, 0, 1)); }

void Player::erasePowerups()
{
    powerups.erase(
        std::remove_if(powerups.begin(), powerups.end(),
                       [](const Powerup &pu)
                       {
                           return pu.getIsDead();
                       }),
        powerups.end());
}

void Player::usePowerup(int index)
{
    Powerup &pu = powerups[index];

    attackType = pu.getType();
    isUsingPowerup = true;
    endUsingPowerup = pu.getEndUsing();

    powerups.erase(powerups.begin() + index);
}

void Player::updatePowerup(uint64_t timer)
{
    if (isUsingPowerup && timer >= endUsingPowerup)
    {
        isUsingPowerup = false;
        attackType = AttackType::Bubble;
    }
}

void Player::pickUpPowerup(std::vector<Powerup>::iterator pu)
{
    int s = powerups.size();
    powerups.insert(powerups.begin(), std::move(*pu));
    while (s > 2)
    {
        powerups.pop_back();
    }
    powerups.front().setX(95 + s * 50);
    powerups.front().setY(93);
}

uint64_t Player::getAttackStartup(AttackType type)
{
    switch (type)
    {
    case AttackType::Sword:
        return 12;
    case AttackType::Shot:
        return 0;
    case AttackType::Bubble:
        return 0;
    default:
        return 0;
    }
}
