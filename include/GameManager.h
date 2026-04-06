#pragma once
#include <vector>
#include <memory>
#include "BaseObject.h"
#include "Player.h"
#include <3ds.h>

class GameManager
{
protected:
    int a;
    std::vector<std::unique_ptr<BaseObject>> objects;

public:
    C3D_RenderTarget *topRight;
    GameManager(int state);
    void init();
    void exit();
    void update(int &s);
    void draw();
    std::vector<std::unique_ptr<BaseObject>> &get_objects();
    long long getTime();
    Player player;
};