#pragma once
#include <citro2d.h>
#include <math.h>
#include "HitBox.h"
#include <string>
#include "Core.h"

class BaseObject
{
protected:
    float x, y;
    float height, width;
    float spawnX;
    float spawnY;
    float speed;
    float vy = 0.0f;
    float vx = 0.0f;
    bool isDead = false;
    C2D_SpriteSheet sheet;
    C2D_Image currentImage = {};

public:
    BaseObject() {}
    BaseObject(float x, float y, float height, float width)
        : x(x), y(y), height(height), width(width), spawnX(x), spawnY(y), speed(0.0f), hitbox(height, width) {}
    BaseObject(float x, float y, float height, float width, float speed)
        : x(x), y(y), height(height), width(width), spawnX(x), spawnY(y), speed(speed), hitbox(height, width) {}
    virtual ~BaseObject() = default;

    HitBox hitbox;

    float getX() const { return x; }
    float getY() const { return y; }
    float getSpawnX() const { return spawnX; }
    float getSpawnY() const { return spawnY; }
    float getCentreX() const { return x + width / 2; }
    float getCentreY() const { return y + height / 2; }
    void changeX(float value) { x += value; }
    void changeY(float value) { y += value; }
    void setX(float value) { x = value; }
    void setY(float value) { y = value; }
    float getDXtoObj(float objCentreX) const { return objCentreX - getCentreX(); }
    float getDYtoObj(float objCentreY) const { return objCentreY - getCentreY(); }
    float distToObj(float objCentreX, float objCentreY) const
    {
        return std::sqrtf(std::powf(getDXtoObj(objCentreX), 2.0f) + std::powf(getDYtoObj(objCentreY), 2.0f));
    }
    float getVX() const { return vx; }
    float getVY() const { return vy; }
    void setNullVX() { vx = 0.0f; }
    void setNullVY() { vy = 0.0f; }
    float getSpeed() const { return speed; }
    float getHeight() const { return height; }
    float getWidth() const { return width; }
    bool getIsDead() const { return isDead; }
    void setIsDead(bool flag) { isDead = flag; }

    void loadSheet(std::string fileName);
    void freeSheet();

    void setImage(C2D_Image newImage);

    C2D_SpriteSheet *getSheetPtr() { return &this->sheet; }
    void setSheetPtr(C2D_SpriteSheet newSheet) { this->sheet = newSheet; }
    C2D_Image *getImagePtr() { return &this->currentImage; }
    virtual void draw(float cameraPos) = 0;
    virtual void update() {}
};