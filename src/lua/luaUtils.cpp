#include "luaUtils.h"

namespace Blink {
    void printLua(lua_State* L, const std::string& tag) {
        if (!tag.empty()) {
            printf("%s\n", tag.c_str());
        }
        printf("-- lua stack\n");
        int stackSize = lua_gettop(L);
        if (stackSize == 0) {
            printf("--   empty\n");
        } else {
            for (int i = 1; i <= stackSize; i++) {
                int stackIndex = i * -1;
                printf("--   [%d] ", stackIndex);
                if (lua_isuserdata(L, stackIndex)) {
                    printf("userdata\n");
                } else if (lua_islightuserdata(L, stackIndex)) {
                    printf("light userdata\n");
                } else if (lua_istable(L, stackIndex)) {
                    printf("table\n");
                } else if (lua_isfunction(L, stackIndex)) {
                    printf("function\n");
                } else if (lua_iscfunction(L, stackIndex)) {
                    printf("cfunction\n");
                } else if (lua_isthread(L, stackIndex)) {
                    printf("thread\n");
                } else if (lua_isnumber(L, stackIndex)) {
                    printf("number\n");
                } else if (lua_isstring(L, stackIndex)) {
                    printf("string\n");
                } else if (lua_isnone(L, stackIndex)) {
                    printf("none\n");
                } else if (lua_isnil(L, stackIndex)) {
                    printf("nil\n");
                } else {
                    printf("unknown type\n");
                }
            }
        }
        printf("-- end\n");
    }

    void printLuaShort(lua_State* L, const std::string& tag) {
        if (!tag.empty()) {
            printf("%s\n", tag.c_str());
        }
        int stackSize = lua_gettop(L);
        if (stackSize == 0) {
            printf("- empty\n");
            return;
        }
        for (int i = 1; i <= stackSize; i++) {
            int stackIndex = i * -1;
            printf("- [%d] ", stackIndex);
            if (lua_isuserdata(L, stackIndex)) {
                printf("userdata\n");
            } else if (lua_islightuserdata(L, stackIndex)) {
                printf("light userdata\n");
            } else if (lua_istable(L, stackIndex)) {
                printf("table\n");
            } else if (lua_isfunction(L, stackIndex)) {
                printf("function\n");
            } else if (lua_iscfunction(L, stackIndex)) {
                printf("cfunction\n");
            } else if (lua_isthread(L, stackIndex)) {
                printf("thread\n");
            } else if (lua_isnumber(L, stackIndex)) {
                printf("number\n");
            } else if (lua_isstring(L, stackIndex)) {
                printf("string\n");
            } else if (lua_isnone(L, stackIndex)) {
                printf("none\n");
            } else if (lua_isnil(L, stackIndex)) {
                printf("nil\n");
            } else {
                printf("unknown type\n");
            }
        }
    }

    void LuaFunction::invoke(lua_State* L) const {
        if (!tableName.empty()) {
            lua_getglobal(L, tableName.c_str());
            lua_getfield(L, -1, name.c_str());
        } else if (!name.empty()) {
            lua_getglobal(L, name.c_str());
        } else if (onGetFunction) {
            onGetFunction(L);
        }
        if (onPushArguments) {
            onPushArguments(L);
        }
        if (lua_pcall(L, argumentCount, returnValueCount, messageHandlerIndex) != LUA_OK) {
            if (onError) {
                onError(L);
            } else {
                BL_LOG_ERROR("Could not invoke Lua function [{}]: {}", name.c_str(), lua_tostring(L, -1));
            }
        }
    }
}
