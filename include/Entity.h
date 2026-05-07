#pragma once
#include "BaseObject.h"
#include "Ground.h"

class Entity : public BaseObject
{
protected:
    int view = 1;
    float vy = 0.0f;
    float vx = 0.0f;
    int hp;
    float speed;
    float damage;
    int lastHitX = 0;
    int lastHitY = 0;
    Ground *groundPlatform = nullptr;

public:
    Entity() {}
    // Entity(float x, float y, float height, float width, int hp, float speed)
    //     : BaseObject(x, y, height, width), hp(hp), speed(speed) {};
    Entity(float x, float y, float height, float width, int hp, float speed, float damage = 0.0f)
        : BaseObject(x, y, height, width), hp(hp), speed(speed), damage(damage) {};

    int getHP() const { return hp; };
    void addHP(int hp = 1) { this->hp += hp; };
    void subHP(int hp = 1) { this->hp -= hp; };

    float getSpeed() const { return speed; }
    virtual void applyLR() { changeX(vx); }
    virtual void applyUD() { changeY(-vy); }
    virtual void moveLeft() { vx = -speed; }
    virtual void moveRight() { vx = speed; }

    float getVX() { return vx; }
    float getVY() { return vy; }
    void setNullVX() { vx = 0.0f; }
    void setNullVY() { vy = 0.0f; }

    int getLastHitX() { return lastHitX; }
    int getLastHitY() { return lastHitY; }
    void setLastHitX(int normalX) { lastHitX = normalX; }
    void setLastHitY(int normalY) { lastHitY = normalY; }

    Ground *getGroundPlatform() { return groundPlatform; }
    void setGroundPlatform(Ground *ground) { groundPlatform = ground; }
    void resetGroundPlatform() { groundPlatform = nullptr; }
    virtual void landOnGround(Ground *ground)
    {
        // onGround = true;
        vy = 0.0f;
        y = ground->getY() - height;
        groundPlatform = ground;
    }
};
