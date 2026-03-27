#pragma once
#include <vector>
#include <memory>
#include "BaseObject.h"

class GameManager
{
private:
    int a;
    std::vector<std::unique_ptr<BaseObject>> objects;

public:
    GameManager(int state);
    void init();
    void exit();
    void update(int &s);
    void draw(C3D_RenderTarget *target);
    std::vector<std::unique_ptr<BaseObject>> &get_objects();
    long long getTime();
};