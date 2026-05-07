#pragma once
#include <citro2d.h>
#include "HitBox.h"
#include <string>
class BaseObject
{
protected:
    float x, y;
    float height, width;
    float spawnX;
    float spawnY;
    C2D_SpriteSheet sheet;
    C2D_Image currentImage = {};

public:
    BaseObject() {}
    BaseObject(float x, float y, float height, float width)
        : x(x), y(y), height(height), width(width), spawnX(x), spawnY(y), hitbox(height, width) {}
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
    void setX(float x) { this->x = x; }
    void setY(float y) { this->y = y; }

    float getHeight() const { return height; }
    float getWidth() const { return width; }

    void loadSheet(std::string fileName);
    void freeSheet();

    void setImage(C2D_Image newImage);

    C2D_SpriteSheet *getSheetPtr() { return &this->sheet; }
    void setSheetPtr(C2D_SpriteSheet newSheet) { this->sheet = newSheet; }
    C2D_Image *getImagePtr() { return &this->currentImage; }
    virtual void draw() = 0;
    virtual void update() {}
};