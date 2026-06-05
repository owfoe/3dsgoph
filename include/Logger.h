#pragma once
#include <iostream>
#include "Core.h"

class Logger
{
public:
    template <typename... Args>
    static void log(Args &&...args)
    {
        if (ProjectSettings::DEBUG && ProjectSettings::CONSOLE)
            print(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void info(Args &&...args)
    {
        if (ProjectSettings::DEBUG && ProjectSettings::CONSOLE)
            std::cout << "[I] ";
        print(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void warn(Args &&...args)
    {
        if (ProjectSettings::DEBUG && ProjectSettings::CONSOLE)
            std::cout << "[W] ";
        print(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void err(Args &&...args)
    {
        if (ProjectSettings::DEBUG && ProjectSettings::CONSOLE)
            std::cout << "[E] ";
        print(std::forward<Args>(args)...);
    }

private:
    template <typename T>
    static void print(T &&arg)
    {
        std::cout << arg << '\n';
    }

    template <typename T, typename... Args>
    static void print(T &&first, Args &&...rest)
    {
        std::cout << first << " ";
        print(std::forward<Args>(rest)...);
    }
};