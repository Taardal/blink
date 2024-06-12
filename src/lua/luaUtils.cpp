#include "luaUtils.h"

namespace Blink {
    int printLuaStacktrace(lua_State* L) {
        std::cerr << "--------------------------------------------------------------------------------------------------------------" << std::endl;
        std::cerr << "[Lua error] " << lua_tostring(L, -1) << std::endl;
        std::cerr << "--------------------------------------------------------------------------------------------------------------" << std::endl;

        lua_Debug debugInfo;
        uint32_t stackLevel = 0;

        // Populate lua_Debug with information about the interpreter runtime stack
        // Level 0 is the current running function, whereas level n+1 is the function that has called level n (except for tail calls, which do not count in the stack).
        // When called with a level greater than the stack depth, lua_getstack returns 0; otherwise it returns 1.
        while (lua_getstack(L, stackLevel, &debugInfo) > 0) {

            // Get information about a specific function or function invocation.
            // - [n] fills in the field name and namewhat;
            // - [S] fills in the fields source, short_src, linedefined, lastlinedefined, and what;
            // - [l] fills in the field currentline;
            // - [t] fills in the field istailcall;
            // - [u] fills in the fields nups, nparams, and isvararg;
            // - [f] pushes onto the stack the function that is running at the given level;
            // - [L] pushes onto the stack a table whose indices are the numbers of the lines that are valid on the function.
            //      - A valid line is a line with some associated code, that is, a line where you can put a break point.
            //      - Non-valid lines include empty lines and comments.
            lua_getinfo(L, "nSltuf", &debugInfo);

            // A reasonable name for the given function.
            // Because functions in Lua are first-class values, they do not have a fixed name.
            // The lua_getinfo function checks how the function was called to find a suitable name.
            // If it cannot find a name, then name is set to NULL.
            std::string name = debugInfo.name ? debugInfo.name : "";

            // Explains the name field according to how the function was called.
            // Possible values:
            // - [global]
            // - [local]
            // - [method]
            // - [field]
            // - [upvalue]
            // - [""] (Empty string when no other option seems to apply)
            std::string namewhat = debugInfo.namewhat != nullptr ? debugInfo.namewhat : "";

            // What type of function it is
            // - [Lua] if the function is a Lua function
            // - [C] if it is a C function
            // - [main] if it is the main part of a chunk
            std::string what = debugInfo.what != nullptr ? debugInfo.what : "";

            // The source of the chunk that created the function.
            // If the source starts with...
            // - [@] The function was defined in a file where the file name follows the '@'
            // - [=] The remainder of its contents describes the source in a user-dependent manner.
            // - [ ] The function was defined in a string where source is that string.
            std::string source = debugInfo.source != nullptr ? debugInfo.source : "";

            // A "printable" version of source, to be used in error messages.
            std::string shortSource = strlen(debugInfo.short_src) > 0 ? debugInfo.short_src : "";

            // The current line where the given function is executing. When no line information is available, currentline is set to -1.
            int currentLine = debugInfo.currentline;

            // The line number where the definition of the function starts.
            int lineDefined = debugInfo.linedefined;

            // The line number where the definition of the function ends.
            int lastLineDefined = debugInfo.lastlinedefined;

            // The number of upvalues of the function.
            int numberOfUpvalues = debugInfo.nups;

            // The number of parameters of the function (always 0 for C functions).
            int numberOfParameters = debugInfo.nparams;

            // True if the function is a variadic function (always true for C functions).
            bool variadicFunction = debugInfo.isvararg;

            // True if this function invocation was called by a tail call.
            // In this case, the caller of this level is not in the stack.
            bool tailCall = debugInfo.istailcall;

            std::string filename = "";
            size_t sourceLastSlashIndex = source.find_last_of("/");
            if (sourceLastSlashIndex != std::string::npos && sourceLastSlashIndex + 1 < source.length()) {
                filename = source.substr(sourceLastSlashIndex + 1);
            }

            std::stringstream ss;
            if (filename.length() > 0) {
                ss << filename;
            }
            if (currentLine > -1) {
                if (filename.length() > 0) {
                    ss << ":";
                }
                ss << currentLine;
            }
            std::string tag = ss.str();
            std::cerr << "[" << stackLevel << "]" << " " << tag << std::endl;

            std::cerr << "  Source: " << (source.length() > 0 ? source : "--") << std::endl;
            std::cerr << "  Line: " << currentLine << std::endl;
            std::cerr << "  Name: " << (name.length() > 0 ? name : "--") << std::endl;
            std::cerr << "  Description: " << (namewhat.length() > 0 ? namewhat : "--") << std::endl;
            std::cerr << "  Parameters: " << numberOfParameters << std::endl;
            std::cerr << "  Upvalues: " << numberOfUpvalues << std::endl;
            std::cerr << "  Start line: " << lineDefined << std::endl;
            std::cerr << "  End line: " << lastLineDefined << std::endl;
            std::cerr << "  Variadic function: " << (variadicFunction ? "Yes" : "No") << std::endl;
            std::cerr << "  Tail call: " << (tailCall ? "Yes" : "No") << std::endl;

            lua_pop(L, 1); // Remove the function pushed by lua_getstack
            ++stackLevel;
        }
        std::cerr << "--------------------------------------------------------------------------------------------------------------" << std::endl;
        std::cerr << std::endl;

        return 1; // Return the error message
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
}
