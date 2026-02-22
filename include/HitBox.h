#pragma once

class HitBox
{
private:
    float lt, rt, top, btm;

public:
    HitBox(float height, float width)
        : lt(0.0f), rt(width), top(0.0f), btm(height) {}

    float left(float x) { return x + lt; }
    float right(float x) { return x + rt; }
    float top(float y) { return y + top; }
    float bottom(float y) { return y + btm; }
};

inline bool Collisions(HitBox &a, float ax, float ay, HitBox &b, float bx, float by)
{
    return (a.left(ax) <= b.right(bx)) &&
           (a.right(ax) >= b.left(bx)) &&
           (a.top(ay) <= b.bottom(by)) &&
           (a.bottom(ay) >= b.top(by));
}