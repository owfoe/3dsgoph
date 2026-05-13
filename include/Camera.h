#pragma once
#include "BaseObject.h"
#include <string>
class Camera : public BaseObject {
public:
    Camera() {}
    Camera(float x, float y, float height, float width)
        : BaseObject(x, y, height, width) {};

    void draw(float cameraPos) {
        C2D_DrawRectSolid(x, y, 1, width, height, C2D_Color32f(0, 0, 1, 1));
    }
};