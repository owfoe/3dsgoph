#pragma once
#include "BaseObject.h"
#include <string>
class Marker : public BaseObject
{
private:
    float maxPos;

public:
    Marker() {}
    Marker(float x, float y, float height, float width, std::string fileName)
        : BaseObject(x, y, height, width)
    {
        this->BaseObject::loadSheet(fileName);
        C2D_SpriteSheet *spriteSheetPtr = BaseObject::getSheetPtr();
        C2D_SpriteSheet spriteSheet = *spriteSheetPtr;
        this->BaseObject::setImage(C2D_SpriteSheetGetImage(spriteSheet, 0));
    };

    void draw(float cameraPos, int layer)
    {
        C2D_Image *imgPtr = this->BaseObject::getImagePtr();
        C2D_Image image = *imgPtr;
        C2D_DrawImageAt(image, x, y, layer);
    }
    void setMaxPos(float x)
    {
        maxPos = x;
    }
    using BaseObject::update;
    void update(float playerPos)
    {
        double formula = Const::MARKER_START_X + playerPos * (Const::MARKER_FORMULA_CONST / maxPos);
        if (formula <= 275)
        {
            setX(Const::MARKER_START_X + playerPos * (Const::MARKER_FORMULA_CONST / maxPos));
        }
    }
};