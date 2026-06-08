#pragma once
#include "Entity.h"
#include "Raycast.h"
#include "FallLogic.h"
#include "Powerup.h"
#include <algorithm>

class Player : public Entity
{
private:
    // bool onGround = true;
    bool isJump = false;
    // bool isFall = true;

    float jumpVelocity = 10.0f;
    float jumpGravityLow = 0.25f;
    float jumpGravityHigh = 0.45f;
    float scaleX = 1.0f;
    // float gravity = 0.5f;
    // float maxFallSpeed = 10.0f;
    int charge = 0;
    int startCharging = 0;

    EntityActionState state = EntityActionState::Stay;

    std::vector<Powerup> powerups;
    bool isUsingPowerup = false;
    uint64_t endUsingPowerup;

public:
    Player() {}
    Player(float x, float y);

    // Raycast raycast;
    FallLogic fallLogic;

    void jump();
    void draw(float cameraPos, int layer) override;

    using Entity::update;
    void update(bool jumpButtonDown);
    void updateJump(bool jumpButtonDown);

    void landOnGround(Ground *ground) override
    {
        Entity::landOnGround(ground);
        isJump = false;
        fallLogic.setIsFall(false);
    }

    void setIsJump(bool flag) { isJump = flag; }
    bool getIsJump() { return isJump; }

    void attackBubble();
    void attackShot();
    void attackSword();
    void pickUpPowerup(std::vector<Powerup>::iterator pu);

    std::vector<Powerup> &getPowerups() { return powerups; }
    void erasePowerups();
    void usePowerup(int index);
    void updatePowerup(uint64_t timer);
    uint64_t getAttackStartup(AttackType type) override;
};
