#pragma once
#include "Entity.h"
#include "Raycast.h"

class Ground;

class Player : public Entity
{
private:
    float prevX;
    float prevY;
    float vy = 0.0f;
    float vx = 0.0f;
    bool onGround = true;
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

    Raycast raycast;

    void jump();
    void draw() override;
    // void moveLeft() override;
    // void moveRight() override;
    void moveLeft();
    void moveRight();
    // void moveLR(float v) override;
    void action(u32 kDown, u32 kHeld);

    using BaseObject::update;
    void update(bool jumpButtonDown);
    void updateJump(bool jumpButtonDown);
    void updateFall();
    void updateFallOnGround(Ground &ground);
    void handleConflict(float hitX, float hitY);
    void updatePosition()
    {
        Entity::moveUD(vy);
        Entity::moveLR(vx);
    }

    void
    setOnGround(bool flag)
    {
        onGround = flag;
    }
    void setIsJump(bool flag) { isJump = flag; }
    void setIsFall(bool flag) { isFall = flag; }
    float getVX() { return vx; }
    float getVY() { return vy; }
    void setNullVX() { vx = 0.0f; }
    void setNullVY() { vy = 0.0f; }

    float getPrevX() { return prevX; }
    float getPrevY() { return prevY; }
    void setPrevXeqX() { prevX = x; }
    void setPrevYeqY() { prevY = y; }
    void setXeqPrevX() { x = prevX; }
    void setYeqPrevY() { y = prevY; }

    bool getIsFall() { return isFall; }
    bool getIsOnGround() { return onGround; }
};
