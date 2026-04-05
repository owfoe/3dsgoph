#include "GameManager.h"
#include <3ds.h>
#include <iostream>
#include <string>
#include <chrono>
#include "Ground.h"
#include "Constants.h"

GameManager::GameManager(int state) {}

void GameManager::init()
{
    gfxInitDefault();
    romfsInit();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    consoleInit(GFX_TOP, NULL);
    // std::cout << GameManager::getTime() << std::endl;
    std::cout << "start" << std::endl;
    //
    objects.push_back(std::make_unique<Ground>(
        SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 32, 64));
    //
}
void GameManager::exit()
{
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
}
void GameManager::draw(C3D_RenderTarget *target)
{
    std::cout << "23453" << std::endl;

    for (auto &obj : objects)
    {
        obj->draw(target);
        std::cout << "1" << std::endl;
    }
}

void GameManager::update(int &s)
{
    gspWaitForVBlank();
    gfxSwapBuffers();
    hidScanInput();
    u32 kDown = hidKeysDown();
    if (kDown & KEY_START)
        s = -1;
}

std::vector<std::unique_ptr<BaseObject>> &GameManager::get_objects()
{
    return objects;
}

long long GameManager::getTime()
{
    auto now = std::chrono::system_clock::now();

    auto duration = now.time_since_epoch();

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                            duration)
                            .count();

    return milliseconds;
}
