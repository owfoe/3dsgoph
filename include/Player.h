#pragma once
#include "Entity.h"
#include "Raycast.h"
#include "FallLogic.h"

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

public:
    Player() {}
    Player(float x, float y, float height, float width);

    // Raycast raycast;
    FallLogic fallLogic;

    void jump();
    void draw(float cameraPos) override;
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

    using BaseObject::update;
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
};