#pragma once

#include <lua.hpp>
#include <string>

namespace Blink {
    int printLuaStacktrace(lua_State* L);

    void printLua(lua_State* L, const std::string& tag);

    void printLuaShort(lua_State* L, const std::string& tag = "");
}