#pragma once
#include "BaseObject.h"
#include <string>
class LowerScreen : public BaseObject
{
public:
    LowerScreen() {}
    LowerScreen(float x, float y, float height, float width, std::string fileName)
        : BaseObject(x, y, height, width, 0.0f)
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
        C2D_DrawImageAt(image, 0, 0, 0);
    }
};