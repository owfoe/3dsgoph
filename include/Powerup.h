#pragma once
#include "BaseObject.h"

class Powerup : public BaseObject
{
private:
    AttackType type;
    bool isUsing = false;
    bool isPickedUp = false;
    uint64_t startUsing;
    uint64_t endUsing;
    uint64_t duration;

public:
    Powerup() {}
    Powerup(float x, float y, float height, float width, AttackType type, uint64_t duration)
        : BaseObject(x, y, height, width), type(type), duration(duration),
          clickHitBox(-Const::POWERUP_DIFF_HITBOX, -Const::POWERUP_DIFF_HITBOX, height + 2 * Const::POWERUP_DIFF_HITBOX, width + 2 * Const::POWERUP_DIFF_HITBOX) {
        if (type == AttackType::Shot) {
            fileName = Path::PWUP_NUT_SHEET;
        }
        else if (type == AttackType::Sword) {
            fileName = Path::PWUP_SWD_SHEET;
        }
        this->BaseObject::loadSheet(fileName);
        this->BaseObject::setImage(C2D_SpriteSheetGetImage(sheet, 0));

    }

    HitBox clickHitBox;
    bool getIsPickedUp() { return isPickedUp; }
    bool getIsUsing() { return isUsing; }
    uint64_t getEndUsing() { return endUsing; }
    AttackType getType() { return type; }

    void setIsUsing(bool flag) { isUsing = flag; }

    void pickUp() { isPickedUp = true; }
    void use(uint64_t timer)
    {
        if (!isUsing)
        {
            startUsing = timer;
            endUsing = startUsing + duration;
            isUsing = true;
        }
    }
    void checkDeath(uint64_t timer)
    {
        if (isUsing)
            isDead = (endUsing <= timer) ? true : false;
    }
    void draw(float cameraPos, int layer) override
    {
        //C2D_DrawRectSolid(x - cameraPos, y, layer, width, height, C2D_Color32f(128, 0, 128, 1.0));

        C2D_DrawImageAt(currentImage, x - cameraPos, y, layer, nullptr, 1.25f, 1.25f);
    }
    using BaseObject::update;
    void update(uint64_t timer)
    {
        if (!isPickedUp)
        {
            float omega = Const::POWERUP_SPEED / Const::POWERUP_RADIUS;
            y = spawnY + Const::POWERUP_RADIUS * std::sinf(timer * omega);
        }
        checkDeath(timer);
    }
};