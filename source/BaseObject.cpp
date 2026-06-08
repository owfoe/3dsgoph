#include "BaseObject.h"
#include <3ds.h>
#include <citro2d.h>
#include <string>


void BaseObject::loadSheet(std::string fileName) {
    C2D_SpriteSheet loadedSheet = C2D_SpriteSheetLoad(fileName.c_str());
    sheet = loadedSheet;

}
void BaseObject::freeSheet() {
    C2D_SpriteSheetFree(sheet);
}


void BaseObject::setImage(C2D_Image newImage) { this->currentImage = newImage; }
