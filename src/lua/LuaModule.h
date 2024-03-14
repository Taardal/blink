#pragma once

#include "LuaEngine.h"
#include "window/WindowModule.h"

namespace Blink {
    struct LuaModule {
    public:
        LuaEngine* luaEngine = nullptr;

    public:
        explicit LuaModule(WindowModule* windowModule);

        ~LuaModule();

        bool initialize();

        void terminate();
    };
}
