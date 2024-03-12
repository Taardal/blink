#pragma once

#include <lua/lua.hpp>

namespace Blink {
    class LuaBinding {
    protected:
        lua_State* L;
        std::string tableName;

    protected:
        LuaBinding(lua_State* L, const std::string& tableName);

    public:
        void update();
    };
}
