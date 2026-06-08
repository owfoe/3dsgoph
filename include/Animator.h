#pragma once
#include <citro2d.h>
#include <unordered_map>
#include <string>

enum AnimMode {
    LOOP,
    ONCE
};

struct Animation {
    C2D_SpriteSheet sheet;
    size_t frameCount;
    int animSpeed;
    AnimMode mode;
};

class Animator
{
private:
    std::unordered_map<std::string, Animation> animations;

    Animation* current = nullptr;

    size_t frame = 0;
    int timer = 0;

    bool finished = false;
public:
    void add(const std::string& name, C2D_SpriteSheet sheet, int speed, AnimMode mode)
    {
        animations[name] = { sheet, C2D_SpriteSheetCount(sheet), speed, mode };
    }

    void play(const std::string& name) {
        auto it = animations.find(name);
        if (it == animations.end())
            return; // or assert

        if (!current || current != &it->second)
        {
            current = &it->second;
            frame = 0;
            timer = 0;
            finished = false;
        }
    }
    void update()
    {
        if (current->frameCount == 0)
            return;

        timer++;
        if (timer >= current->animSpeed)
        {
            timer = 0;
            if (frame < current->frameCount - 1)
            {
                frame += 1;
            }
            else
            {
                if (current->mode == LOOP)
                    frame = 0;
                else
                    finished = true;
            }
        }
    }

    C2D_Image getImage()
    {
        if (!current || current->frameCount == 0)
            return C2D_Image{};

        return C2D_SpriteSheetGetImage(current->sheet, frame);
    }
    void exit() {
        animations.clear();
        current = nullptr;
    }
    bool isFinished() const { return finished; }
};