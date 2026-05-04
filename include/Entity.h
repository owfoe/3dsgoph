#pragma once
#include "BaseObject.h"

class Ground;

class Entity : public BaseObject
{
protected:
    float vy = 0.0f;
    float vx = 0.0f;
    int hp;
    float speed;
    float damage;
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
    virtual void moveLR() { changeX(vx); }
    virtual void moveUD() { changeY(-vy); }

    float getVX() { return vx; }
    float getVY() { return vy; }
    void setNullVX() { vx = 0.0f; }
    void setNullVY() { vy = 0.0f; }

    Ground *getGroundPlatform() { return groundPlatform; }
    void setGroundPlatform(Ground *ground) { groundPlatform = ground; }
};
