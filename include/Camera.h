#pragma once
#include "BaseObject.h"
class Camera : public BaseObject
{
private:
    int frameSpeed;
    float smoothing;

public:
    Camera() {}
    Camera(float x, float y, float height, float width, float smoothing)
        : BaseObject(x, y, height, width), smoothing(smoothing) {}
    void draw(float cameraPos, int layer)
    {
        C2D_DrawRectSolid(x, y, 1, width, height, C2D_Color32f(0, 0, 1, 1));
    }
    int getFrameSpeed() { return frameSpeed; }
    void setFrameSpeed(int fs) { frameSpeed = fs; }

    float getSmoothing() { return smoothing; }
};