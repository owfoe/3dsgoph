#pragma once
#include <iostream>

class HitBox
{
protected:
    float lt, rt, top, btm;

public:
    HitBox() {}
    HitBox(float height, float width)
        : lt(0.0f), rt(width), top(0.0f), btm(height) {}
    HitBox(float top, float lt, float height, float width)
        : lt(lt), rt(lt + width), top(top), btm(top + height) {}

    float leftB(float x) { return x + lt; }
    float rightB(float x) { return x + rt; }
    float topB(float y) { return y + top; }
    float bottomB(float y) { return y + btm; }
    void draw(float x, float y)
    {
        C2D_DrawRectSolid(leftB(x), topB(y), 0, rt - lt, btm - top, C2D_Color32f(0, 0, 1, 1));
    }
    void printBorders(float x, float y)
    {
        std::cout << leftB(x) << " " << topB(y) << " " << rightB(x) << " " << bottomB(y) << std::endl;
    }
};

inline bool AABB(HitBox &a, float ax, float ay, HitBox &b, float bx, float by)
{
    return (a.leftB(ax) < b.rightB(bx)) &&
           (a.rightB(ax) > b.leftB(bx)) &&
           (a.topB(ay) <= b.bottomB(by)) &&
           (a.bottomB(ay) >= b.topB(by));
}

// inline bool AABBTopAndBottom(HitBox &a, float ax, float ay, HitBox &b, float bx, float by)
// {
//     return (a.leftB(ax) >= b.leftB(bx)) &&
//            (a.rightB(ax) <= b.rightB(bx)) &&
//            (a.topB(ay) <= b.bottomB(by)) &&
//            (a.bottomB(ay) >= b.topB(by));
// }

struct CollisionResult
{
    bool hit = false;
    float hitX = 0.0f;
    float hitY = 0.0f;
    float normalX = 0.0f;
    float normalY = 0.0f;
};

inline CollisionResult sweptAABB(HitBox &a, float ax, float ay, float vx, float vy,
                                 HitBox &b, float bx, float by)
{
    CollisionResult result;

    float aLeft = a.leftB(ax);
    float aRight = a.rightB(ax);
    float aTop = a.topB(ay);
    float aBottom = a.bottomB(ay);

    float bLeft = b.leftB(bx);
    float bRight = b.rightB(bx);
    float bTop = b.topB(by);
    float bBottom = b.bottomB(by);

    float xEntry, yEntry;
    float xExit, yExit;

    if (vx > 0.0f)
    {
        xEntry = (bLeft - aRight) / vx;
        xExit = (bRight - aLeft) / vx;
    }
    else if (vx < 0.0f)
    {
        xEntry = (bRight - aLeft) / vx;
        xExit = (bLeft - aRight) / vx;
    }
    else
    {
        xEntry = -INFINITY;
        xExit = INFINITY;
    }

    if (vy > 0.0f)
    {
        yEntry = (bBottom - aTop) / vy;
        yExit = (bTop - aBottom) / vy;
    }
    else if (vy < 0.0f)
    {
        yEntry = (bTop - aBottom) / vy;
        yExit = (bBottom - aTop) / vy;
    }
    else
    {
        yEntry = -INFINITY;
        yExit = INFINITY;
    }

    float entryTime = std::max(xEntry, yEntry);
    float exitTime = std::min(xExit, yExit);
    // std::cout << "entryTime: " << entryTime << std::endl;
    // std::cout << "exitTime: " << exitTime << std::endl;
    // // std::cout << "xEntry: " << xEntry << std::endl;
    // std::cout << "yEntry: " << yEntry << std::endl;
    // // std::cout << "xExit: " << xExit << std::endl;
    // std::cout << "yExit: " << yExit << std::endl;

    if (entryTime > exitTime || (xEntry < 0.0f && yEntry < 0.0f) || xEntry > 1.0f || yEntry > 1.0f)
        return result;

    result.hit = true;
    if (xEntry > yEntry)
    {
        result.hitX = (vx > 0.0f) ? bLeft - (aRight - aLeft) : bRight;
        result.normalX = (vx > 0.0f) ? -1.0f : 1.0f;
    }
    else
    {
        result.hitY = (vy > 0.0f) ? bBottom : bTop - (aBottom - aTop);
        result.normalY = (vy > 0.0f) ? 1.0f : -1.0f;
    }

    return result;
}
