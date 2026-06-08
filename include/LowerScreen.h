#pragma once
#include "BaseObject.h"
#include <string>
class LowerScreen : public BaseObject
{
public:
    LowerScreen() {}
    LowerScreen(float x, float y, float height, float width, std::string fileName)
        : BaseObject(x, y, height, width)
    {
        this->BaseObject::loadSheet(fileName);
        this->BaseObject::setImage(C2D_SpriteSheetGetImage(sheet, 0));
    };

    void draw(float cameraPos, int layer)
    {
        C2D_DrawImageAt(currentImage, 0, 0, layer);
    }
};