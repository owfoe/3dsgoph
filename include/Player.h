#pragma once
#include "Entity.h"
#include "Raycast.h"
#include "FallLogic.h"
#include "Powerup.h"

class Player : public Entity
{
private:
    // bool onGround = true;
    bool isJump = false;
    // bool isFall = true;

    float jumpVelocity = 10.0f;
    float jumpGravityLow = 0.25f;
    float jumpGravityHigh = 0.45f;
    // float gravity = 0.5f;
    // float maxFallSpeed = 10.0f;
    int charge = 0;
    int startCharging = 0;

    enum class State
    {
        Stay,
        Run,
        Attack
    };
    State state = State::Stay;

    std::vector<Powerup *> powerups;

public:
    Player() {}
    Player(float x, float y, float height, float width);

    // Raycast raycast;
    FallLogic fallLogic;

    void jump();
    void draw(float cameraPos, int layer) override;
    void moveLeft() override
    {
        Entity::moveLeft();
        view = -1;
    }
    void moveRight() override
    {
        Entity::moveRight();
        view = 1;
    }

    using Entity::update;
    void update(bool jumpButtonDown);
    void updateJump(bool jumpButtonDown);

    void landOnGround(Ground *ground) override
    {
        Entity::landOnGround(ground);
        isJump = false;
        fallLogic.setIsFall(false);
    }

    // void setOnGround(bool flag) { onGround = flag; }
    void setIsJump(bool flag) { isJump = flag; }
    // bool getIsOnGround() { return onGround; }
    bool getIsJump() { return isJump; }

    void attack(std::vector<Projectile> &projectiles, uint64_t timer) override;
    void chargeAttack(uint64_t timer)
    {
        if (startCharging == 0)
            startCharging = timer;
    }
    void pickUpPowerup(Powerup *pu)
    {
        int s = powerups.size();
        if (s == 2)
            powerups.at(1)->setIsDead(true);
        pu->setX(95 + s * 50);
        pu->setY(93);
        powerups.insert(powerups.begin(), pu);
    }

    std::vector<Powerup *> getPowerups() { return powerups; }
};
