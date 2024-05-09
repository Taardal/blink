#pragma once

#include "window/Keyboard.h"

#include <lua.hpp>

namespace Blink {
    class KeyboardLuaBinding {
    private:
        Keyboard* keyboard;

    public:
        explicit KeyboardLuaBinding(Keyboard* keyboard);

        static void initialize(lua_State* L, Keyboard* keyboard);

    private:
        static void createKeyboardTable(lua_State* L, Keyboard* keyboard);

        static void createKeyTable(lua_State* L, Keyboard* keyboard);

        static int destroy(lua_State* L);

        static int index(lua_State* L);

        static int isPressed(lua_State* L);
    };
}
