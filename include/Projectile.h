#pragma once
#include "BaseObject.h"
#include <string>
class Projectile : public BaseObject
{
private:
    C2D_SpriteSheet *spriteSheetPtr;
    C2D_SpriteSheet spriteSheet;
    OwnerType owner;
    AttackType type;
    float targetX = 0.0f;
    float targetY = 0.0f;
    float power = 0.0f;
    uint64_t spawnTime;
    int view = 0;
    bool isHeavy;

public:
    Projectile() {}
    Projectile(float x, float y, float height, float width, float speed, OwnerType owner, uint64_t spawnTime, AttackType type, float targetX, float targetY)
        : BaseObject(x, y, height, width, speed), owner(owner), type(type), targetX(targetX), targetY(targetY), spawnTime(spawnTime)
    {
        if (type == AttackType::Shot)
        {
            fileName = Path::NUT_SHEET;
            this->BaseObject::loadSheet(fileName);
            frameCount = 0;
            float dx = getDXtoObj(targetX);
            float dy = getDYtoObj(targetY);
            float len = distToObj(targetX, targetY);
            if (len == 0.0f)
                return;
            vx = dx / len * speed;
            vy = dy / len * speed;
        }
    }
    Projectile(float x, float y, float height, float width, float speed, OwnerType owner, uint64_t spawnTime, AttackType type, int view, float power, bool isHeavy)
        : BaseObject(x, y, (isHeavy) ? height * 1.5f : height, (isHeavy) ? width * 1.5f : width, speed), owner(owner), type(type), power(power), spawnTime(spawnTime), view(view), isHeavy(isHeavy)
    {
        if (type == AttackType::Bubble)
        {
            fileName = Path::BUBBLE_SHEET;
            if (isHeavy) {
                fileName = Path::HEAVY_BUBBLE_SHEET;
            }
            this->BaseObject::loadSheet(fileName);
            frameCount = C2D_SpriteSheetCount(sheet);
            vx = speed * (power + 1) * view;
        }
    }

    OwnerType getOwner() { return owner; }
    void draw(float cameraPos, int layer) override {
        this->BaseObject::setImage(C2D_SpriteSheetGetImage(sheet, animFrame));

        // C2D_DrawRectSolid(x - cameraPos, y, layer, width, height, C2D_Color32f(255, 255, 0, 1));

        if (type == AttackType::Shot) C2D_DrawImageAt(currentImage, x - cameraPos, y, layer);

        else {
            if (!isHeavy) C2D_DrawImageAt(currentImage, x - (cameraPos + 2), y - 2, layer, nullptr, 0.5f, 0.5f);
            else if (isHeavy) C2D_DrawImageAt(currentImage, x - (cameraPos + 2), y - 2, layer, nullptr, 0.5f, 0.5f);
            animContinue();
        }
    }
    void update() override
    {
        x += vx;
        y += vy;
        int k = 1;
        if (type == AttackType::Bubble)
        {
            if (isHeavy)
                k *= -1;
            vy -= (0.1f * 1.5f) * k;
        }
    }
};
