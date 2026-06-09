#pragma once
#include <citro2d.h>
#include <unordered_map>
#include <string>
#include "Core.h"

class Animator
{
private:
    std::unordered_map<std::string, Animation> animations;

    std::string current;

    size_t frame = 0;
    int timer = 0;

    bool finished = false;

public:
    void add(const std::string &name, C2D_SpriteSheet sheet, int speed, AnimMode mode)
    {
        animations[name] = {sheet, C2D_SpriteSheetCount(sheet), speed, mode};
    }

    void play(const std::string &name)
    {
        auto it = animations.find(name);
        if (it == animations.end())
            return;

        if (current != name)
        {
            current = name;
            frame = 0;
            timer = 0;
            finished = false;
        }
    }
    void update()
    {
        if (current.empty())
            return;

        auto it = animations.find(current);
        if (it == animations.end())
            return;

        Animation &anim = it->second;
        if (anim.frameCount == 0)
            return;

        timer += 1;
        if (timer >= anim.animSpeed)
        {
            timer = 0;
            if (frame < anim.frameCount - 1)
            {
                frame += 1;
            }
            else
            {
                if (anim.mode == LOOP)
                    frame = 0;
                else
                    finished = true;
            }
        }
    }

    C2D_Image getImage()
    {
        if (current.empty())
            return C2D_Image{};

        auto it = animations.find(current);
        if (it == animations.end())
            return C2D_Image{};

        Animation &anim = it->second;

        if (anim.frameCount == 0)
            return C2D_Image{};

        return C2D_SpriteSheetGetImage(anim.sheet, frame);
    }
    void exit()
    {
        animations.clear();
        current.clear();
        frame = 0;
        timer = 0;
        finished = false;
    }
    bool isFinished() const { return finished; }
};