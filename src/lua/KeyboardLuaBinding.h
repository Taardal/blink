#pragma once

#include "luaUtils.h"
#include "window/Keyboard.h"

#include <lua/lua.hpp>

namespace Blink {
    class KeyboardLuaBinding {
    private:
        Keyboard* keyboard;

    public:
        explicit KeyboardLuaBinding(Keyboard* keyboard);

        static void create(lua_State* L, Keyboard* keyboard);

    private:
        static void createKeyboardTable(lua_State* L, Keyboard* keyboard);

        static void createKeyTable(lua_State* L, Keyboard* keyboard);

        static LuaReturnValueCount destroy(lua_State* L);

        static LuaReturnValueCount index(lua_State* L);

        static LuaReturnValueCount isPressed(lua_State* L);
    };
}
