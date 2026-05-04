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
    Ground *groundPlatform = nullptr;

public:
    Entity() {}
    Entity(float x, float y, float height, float width, int hp, float speed)
        : BaseObject(x, y, height, width), hp(hp), speed(speed) {};

    int getHP() const { return this->hp; };
    void damage(int hp = 1) { this->hp -= hp; };
    void heal(int hp = 1) { this->hp += hp; };

    float getSpeed() const { return this->speed; }
    // virtual void moveLeft() { changeX(-getSpeed()); }
    // virtual void moveRight() { changeX(getSpeed()); }
    virtual void moveLR() { changeX(vx); }
    virtual void moveUD() { changeY(-vy); }

    float getVX() { return vx; }
    float getVY() { return vy; }
    void setVX(float v) { vx = v; }
    void setVY(float v) { vy = v; }
    void setNullVX() { vx = 0.0f; }
    void setNullVY() { vy = 0.0f; }

    Ground *getGroundPlatform() { return groundPlatform; }
    void setGroundPlatform(Ground *ground) { groundPlatform = ground; }
};
