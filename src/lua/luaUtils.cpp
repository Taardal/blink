#include "luaUtils.h"

namespace Blink {
    // 'n': fills in the field name and namewhat;
    // 'S': fills in the fields source, short_src, linedefined, lastlinedefined, and what;
    // 'l': fills in the field currentline;
    // 't': fills in the field istailcall;
    // 'u': fills in the fields nups, nparams, and isvararg;
    // 'f': pushes onto the stack the function that is running at the given level;
    // 'L': pushes onto the stack a table whose indices are the numbers of the lines that are valid on the function. (A valid line is a line with some associated code, that is, a line where you can put a break point. Non-valid lines include empty lines and comments.)
    void printLuaDebugInfo(lua_State* L) {
        lua_Debug ar;

        int level = 0;
        while (lua_getstack(L, level, &ar)) {
            lua_getinfo(L, "nSltufL", &ar);

            std::cout << "Level: " << level << std::endl;
            std::cout << "  Function: " << (ar.name ? ar.name : "nil") << std::endl;
            std::cout << "  Namewhat: " << (ar.namewhat ? ar.namewhat : "nil") << std::endl;
            std::cout << "  Source: " << (ar.source ? ar.source : "nil") << std::endl;
            std::cout << "  Current line: " << ar.currentline << std::endl;
            std::cout << "  Defined line: " << ar.linedefined << std::endl;
            std::cout << "  Num upvalues: " << ar.nups << std::endl;

            ++level;
        }
    }

    void printLuaStack(lua_State* L) {
        int top = lua_gettop(L);  // Get the stack size
        std::cout << "Stack size: " << top << std::endl;

        for (int i = 1; i <= top; ++i) {
            int type = lua_type(L, i);  // Get the type of the value at index i
            const char* typeName = lua_typename(L, type);  // Get the name of the type
            std::cout << "Stack[" << i << "]: " << typeName;

            switch (type) {
                case LUA_TNUMBER:
                    std::cout << " (" << lua_tonumber(L, i) << ")";
                break;
                case LUA_TSTRING:
                    std::cout << " (" << lua_tostring(L, i) << ")";
                break;
                case LUA_TBOOLEAN:
                    std::cout << " (" << (lua_toboolean(L, i) ? "true" : "false") << ")";
                break;
                case LUA_TNIL:
                    std::cout << " (nil)";
                break;
                default:
                    std::cout << " (other)";
                break;
            }

            std::cout << std::endl;
        }
    }

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
                    auto number = (double) lua_tonumber(L, stackIndex);
                    printf("number (%f)\n", number);
                } else if (lua_isstring(L, stackIndex)) {
                    auto string = lua_tostring(L, stackIndex);
                    printf("string (%s)\n", string);
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
        } else {
            BL_THROW("Could not get function to invoke");
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
