#include "BaseObject.h"

class Ground : public BaseObject
{
private:
    bool dynamic;
    float speed;

public:
    Ground(float x, float y, float height, float width)
        : BaseObject(x, y, height, width), dynamic(0), speed(0.0f) {}
    Ground(float x, float y, float height, float width, bool dynamic, float speed)
        : BaseObject(x, y, height, width), dynamic(dynamic), speed(speed) {}

    bool isDynamic() const { return this->dynamic; }
    float getSpeed() const { return this->speed; }
    void moveLeft() { changeX(-getSpeed()); }
    void moveRight() { changeX(getSpeed()); }
    void draw(C3D_RenderTarget *target) override
    {
        C2D_DrawRectSolid(x, y, 0, width, height, C2D_Color32f(1, 0, 0, 1));
    }
};