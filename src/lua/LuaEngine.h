#pragma once

#include <lua/lua.hpp>

namespace Blink {
    class LuaEngine {
    private:
        lua_State* L;

    public:
        LuaEngine();

        ~LuaEngine();

        bool initialize();

        void terminate();
    };
}
