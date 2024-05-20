#pragma once

#include <lua.hpp>
#include <string>

namespace Blink {
    typedef int LuaReturnValueCount;

    void printLuaStack(lua_State* L);

    void printLuaDebugInfo(lua_State* L);

    void printLua(lua_State* L, const std::string& tag);

    void printLuaShort(lua_State* L, const std::string& tag = "");

    struct LuaFunction {
        std::string name;
        std::string tableName;
        int argumentCount = 0;
        int returnValueCount = 0;
        int messageHandlerIndex = 0;
        std::function<void(lua_State*)> onGetFunction = nullptr;
        std::function<void(lua_State*)> onPushArguments = nullptr;
        std::function<void(lua_State*)> onError = nullptr;

        void invoke(lua_State* L) const;
    };
}