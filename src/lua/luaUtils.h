#pragma once

#include <lua/lua.hpp>
#include <string>

typedef int LuaReturnValueCount;

void printLua(lua_State* L, const std::string& tag);

void printLuaShort(lua_State* L, const std::string& tag = "");
