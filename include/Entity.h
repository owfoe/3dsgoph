#pragma once
#include "BaseObject.h"
#include "Ground.h"
#include "Projectile.h"
#include <vector>
#include <Sword.h>

class Entity : public BaseObject
{
protected:
    int view = 1;

    int hp;
    int damage = 1.0f;
    int lastHitX = 0;
    int lastHitY = 0;
    uint64_t cooldown;
    uint64_t lastAttack;
    Ground *groundPlatform = nullptr;

public:
    Entity() {}
    Entity(float x, float y, float height, float width, int hp, float speed, uint64_t cooldown)
        : BaseObject(x, y, height, width, speed), hp(hp), cooldown(cooldown) {};

    int getHP() { return hp; };
    void addHP(int hp = 1) { this->hp += hp; };
    void subHP(int hp = 1) { this->hp -= hp; };

    void checkDeath() { isDead = (hp <= 0) ? true : false; }

    virtual void attack(std::vector<Projectile> &projectiles, uint64_t timer) { lastAttack = timer; }

    virtual void applyLR() { changeX(vx); }
    virtual void applyUD() { changeY(-vy); }
    virtual void moveLeft() { vx = -speed; }
    virtual void moveRight() { vx = speed; }

    int getLastHitX() { return lastHitX; }
    int getLastHitY() { return lastHitY; }
    void setLastHitX(int normalX) { lastHitX = normalX; }
    void setLastHitY(int normalY) { lastHitY = normalY; }

    Ground *getGroundPlatform() { return groundPlatform; }
    void setGroundPlatform(Ground *ground) { groundPlatform = ground; }
    void resetGroundPlatform() { groundPlatform = nullptr; }
    virtual void landOnGround(Ground *ground)
    {
        vy = 0.0f;
        y = ground->getY() - height;
        groundPlatform = ground;
    }
};
