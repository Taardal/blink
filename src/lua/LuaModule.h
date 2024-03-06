#pragma once

#include "LuaEngine.h"

namespace Blink {
    struct LuaModule {
    public:
        LuaEngine* luaEngine = nullptr;

    public:
        LuaModule();

        ~LuaModule();

        bool initialize();

        void terminate();
    };
}
