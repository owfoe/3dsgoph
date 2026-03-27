#pragma once

class HitBox
{
protected:
    float lt, rt, top, btm;

public:
    HitBox(float height, float width)
        : lt(0.0f), rt(width), top(0.0f), btm(height) {}

    float leftB(float x) { return x + lt; }
    float rightB(float x) { return x + rt; }
    float topB(float y) { return y + top; }
    float bottomB(float y) { return y + btm; }
};

// inline bool Collisions(HitBox &a, float ax, float ay, HitBox &b, float bx, float by)
// {
//     return (a.leftB(ax) <= b.rightB(bx)) &&
//            (a.rightB(ax) >= b.leftB(bx)) &&
//            (a.topB(ay) <= b.bottomB(by)) &&
//            (a.bottomB(ay) >= b.topB(by));
// }