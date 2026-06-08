#pragma once
#include "BaseObject.h"
#include "Ground.h"
#include "Projectile.h"
#include <vector>
#include <algorithm>

class Entity : public BaseObject
{
protected:
    int view = 1;

    int hp;
    int maxHP;
    int damage = 1.0f;
    int lastHitX = 0;
    int lastHitY = 0;
    float scaleX;
    Ground *groundPlatform = nullptr;

    uint64_t cooldown;
    uint64_t lastAttack = 0;
    AttackType attackType;
    PendingAttack pendingAttack;

    EntityActionState actionState = EntityActionState::Stay;

public:
    Entity() {}
    Entity(float x, float y, float height, float width, int hp, float speed, uint64_t cooldown, AttackType attackType)
        : BaseObject(x, y, height, width, speed), hp(hp), maxHP(hp), cooldown(cooldown), attackType(attackType) {};

    int getHP() { return hp; }
    void addHP(int hp = 1) { this->hp += hp; }
    void subHP(int hp = 1) { this->hp -= hp; }
    void setHP(int hp) { this->hp = hp; }
    void resetHP() { setHP(maxHP); }

    float getProjectileSpawnX() { return (view == 1) ? x + width : x; }
    float getProjectileSpawnY() { return y + height / 4; }
    AttackType getAttackType() { return attackType; }

    void checkDeath() { isDead = (hp <= 0) ? true : false; }
    bool isObjForward(float objCentreX) { return ((objCentreX - hitbox.rightB(x)) * view >= 0) ? true : false; }

    virtual void applyLR()
    {
        changeX(vx);
        actionState = EntityActionState::Run;
    }
    virtual void applyUD() { changeY(-vy); }
    virtual void moveLeft()
    {
        vx = -speed;
        view = -1;
        actionState = EntityActionState::Run;
    }
    virtual void moveRight()
    {
        vx = speed;
        view = 1;
        actionState = EntityActionState::Run;
    }
    void setNullVX() override
    {
        BaseObject::setNullVX();
        actionState = EntityActionState::Stay;
    }
    EntityActionState getState() { return actionState; }
    void setScaleX(float scale) { scaleX = scale; }
    float getScaleX() { return scaleX; }
    int getLastHitX() { return lastHitX; }
    int getLastHitY() { return lastHitY; }
    void setLastHitX(int normalX) { lastHitX = normalX; }
    void setLastHitY(int normalY) { lastHitY = normalY; }

    Ground *getGroundPlatform() { return groundPlatform; }
    void setGroundPlatform(Ground *ground) { groundPlatform = ground; }
    void resetGroundPlatform() { groundPlatform = nullptr; }
    virtual void landOnGround(Ground *ground)
    {
        setNullVY();
        y = ground->getY() - height;
        groundPlatform = ground;
    }

    virtual uint64_t getAttackStartup(AttackType type);
    bool startAttack(AttackType type, uint64_t timer, float targetX = Const::DEFAULT_X, float targetY = Const::DEFAULT_Y, bool heavyBubble = false, float strength = 0.0f)
    {
        if (pendingAttack.active)
            return false;

        if (timer < lastAttack + cooldown)
            return false;

        pendingAttack.type = type;
        pendingAttack.active = true;
        pendingAttack.attackTime = timer + getAttackStartup(type);
        pendingAttack.view = view;
        pendingAttack.targetX = targetX;
        pendingAttack.targetY = targetY;
        pendingAttack.power = std::clamp(strength * 30.0f, 0.5f, 6.0f);
        pendingAttack.heavyBubble = heavyBubble;

        actionState = EntityActionState::Attack;
        return true;
    }

    bool consumeReadyAttack(uint64_t timer, PendingAttack &out)
    {
        if (!pendingAttack.active)
            return false;

        if (timer < pendingAttack.attackTime)
            return false;

        out = pendingAttack;
        pendingAttack.active = false;
        lastAttack = timer;

        return true;
    }
};
