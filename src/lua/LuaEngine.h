#pragma once

#include <lua/lua.hpp>

namespace Blink {
    struct EntityBinding {};

    class LuaEngine {
    private:
        lua_State* L;

    public:
        LuaEngine();

        ~LuaEngine();

        bool initialize();

        void terminate();

        bool loadFile(const std::string& path);

        void update(const std::string& tableName);
    };
}
