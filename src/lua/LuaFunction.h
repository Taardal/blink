#pragma once

#include <lua/lua.hpp>

namespace Blink {
    struct LuaFunction {
        std::string name;
        std::string tableName;
        int argumentCount = 0;
        int returnValueCount = 0;
        int messageHandlerIndex = 0;
        std::function<void(lua_State*)> onPushArguments = nullptr;
        std::function<void(lua_State*)> onError = nullptr;

        void invoke(lua_State* L) const {
            if (!tableName.empty()) {
                lua_getglobal(L, tableName.c_str());
                lua_getfield(L, -1, name.c_str());
            } else if (!name.empty()) {
                lua_getglobal(L, name.c_str());
            }
            if (onPushArguments) {
                onPushArguments(L);
            }
            if (lua_pcall(L, argumentCount, returnValueCount, messageHandlerIndex) != LUA_OK) {
                if (onError) {
                    onError(L);
                } else {
                    luaL_error(L, "Could not invoke function [%s]: %s", name.c_str(), lua_tostring(L, -1));
                }
            }
        }
    };

}