#pragma once
#include "BaseObject.h"

class Entity : public BaseObject
{
protected:
    int hp;
    float speed;

public:
    Entity() {}
    Entity(float x, float y, float height, float width, int hp, float speed)
        : BaseObject(x, y, height, width), hp(hp), speed(speed) {};

    int getHP() const { return this->hp; };
    void damage(int hp = 1) { this->hp -= hp; };
    void heal(int hp = 1) { this->hp += hp; };

    float getSpeed() const { return this->speed; }
    virtual void moveLeft() { changeX(-getSpeed()); }
    virtual void moveRight() { changeX(getSpeed()); }
};