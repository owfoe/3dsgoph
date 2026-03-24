#include "GameManager.h"
#include <3ds.h>
#include <iostream>
#include <string>
#include <chrono>

GameManager::GameManager(int state) {}

void GameManager::init() {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    std::cout << GameManager::getTime() << std::endl;
}
void GameManager::exit() {
    gfxExit();
    romfsExit();
}
void GameManager::update(int& s) {
    gspWaitForVBlank();
    gfxSwapBuffers();
    hidScanInput();
    u32 kDown = hidKeysDown();
    if (kDown & KEY_START)
        s = -1;
}

long long GameManager::getTime() {
    auto now = std::chrono::system_clock::now();

    auto duration = now.time_since_epoch();

    auto milliseconds
            = std::chrono::duration_cast<std::chrono::milliseconds>(
                  duration)
                  .count();

    return milliseconds;
}