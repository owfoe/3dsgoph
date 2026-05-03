#pragma once
#include "Entity.h"
#include "Raycast.h"

class Ground;

class Player : public Entity
{
private:
    float vy = 0.0f;
    float vx = 0.0f;
    // bool onGround = true;
    bool isJump = false;
    bool isFall = true;

    float jumpVelocity = 10.0f;
    float gravityLow = 0.25f;
    float gravityHigh = 0.45f;
    float gravity = 0.5f;
    float maxFallSpeed = 10.0f;

    Ground *groundPlatform = nullptr;

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

    void applyVX() { Entity::moveLR(vx); }
    void applyVY() { Entity::moveUD(vy); }

    void updatePosition()
    {
        Entity::moveUD(vy);
        Entity::moveLR(vx);
    }

    // void setOnGround(bool flag) { onGround = flag; }
    void setIsJump(bool flag) { isJump = flag; }
    void setIsFall(bool flag) { isFall = flag; }

    float getVX() { return vx; }
    float getVY() { return vy; }
    void setVX(float v) { vx = v; }
    void setVY(float v) { vy = v; }
    void setNullVX() { vx = 0.0f; }
    void setNullVY() { vy = 0.0f; }

    bool getIsFall() { return isFall; }
    // bool getIsOnGround() { return onGround; }
    bool getIsJump() { return isJump; }

    Ground *getGroundPlatform() { return groundPlatform; }
    void setGroundPlatform(Ground *ground) { groundPlatform = ground; }
};
