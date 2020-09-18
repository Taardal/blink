#pragma once

#include <lua/lua.hpp>
#include <string>

void printlua(lua_State* L, const std::string& tag = "");