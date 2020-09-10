#pragma once
#include "LuaEngine.h"

namespace blink
{
    class Application
    {
    private:
        Window* window;
        LuaEngine* luaEngine;
        bool running;

    public:
        Application(Window* window, LuaEngine* luaEngine);

        void Run();
    };
}


