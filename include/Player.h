#pragma once
#include "Entity.h"
#include "Raycast.h"

class Ground;

class Player : public Entity
{
private:
    float vy = 0.0f;
    float vx = 0.0f;
    bool onGround = true;
    bool isJump = false;

    float jumpVelocity = 10.0f;
    float gravityLow = 0.25f;
    float gravityHigh = 0.45f;
    float maxFallSpeed = 10.0f;

public:
    Player() {}
    Player(float x, float y, float height, float width);

    Raycast raycast;

    void jump();
    void draw() override;
    void moveLeft() override;
    void moveRight() override;
    using BaseObject::update;
    void update(bool jumpButtonDown);
    void updateJump(bool jumpButtonDown);
    void updateFall();
    void updateFallOnGround(Ground &ground);

    void setOnGround(bool flag) { onGround = flag; }
    void setIsJump(bool flag) { isJump = flag; }
    void setNullVY() { vy = 0; }
    void setNullVX() { vx = 0; }
};
