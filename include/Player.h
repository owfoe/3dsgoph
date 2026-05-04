#pragma once
#include "Entity.h"
#include "Raycast.h"

class Player : public Entity
{
private:
    // bool onGround = true;
    bool isJump = false;
    bool isFall = true;

    float jumpVelocity = 10.0f;
    float gravityLow = 0.25f;
    float gravityHigh = 0.45f;
    float gravity = 0.5f;
    float maxFallSpeed = 10.0f;

public:
    Player() {}
    Player(float x, float y, float height, float width);

    // Raycast raycast;

    void jump();
    void draw() override;
    void moveLeft();
    void moveRight();
    void action(u32 kDown, u32 kHeld);

    using BaseObject::update;
    void update(bool jumpButtonDown);
    void updateJump(bool jumpButtonDown);
    void updateFall();

    void landOnGround(Ground *ground);
    void resetGroundState();

    void handleConflict(float hitX, float hitY);

    // void setOnGround(bool flag) { onGround = flag; }
    void setIsJump(bool flag) { isJump = flag; }
    void setIsFall(bool flag) { isFall = flag; }

    bool getIsFall() { return isFall; }
    // bool getIsOnGround() { return onGround; }
    bool getIsJump() { return isJump; }
};
