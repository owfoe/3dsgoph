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
        this->BaseObject::setImage(C2D_SpriteSheetGetImage(sheet, 0));
    };

    void draw(float cameraPos, int layer)
    {
        C2D_DrawImageAt(currentImage, x, y, layer);
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