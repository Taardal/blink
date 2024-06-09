#pragma once

#include "window/Mouse.h"

#include <lua.hpp>

namespace Blink {
    class MouseLuaBinding {
    private:
        Mouse* mouse;

    public:
        explicit MouseLuaBinding(Mouse* mouse);

        static void initialize(lua_State* L, Mouse* mouse);

    private:
        static void createMouseTable(lua_State* L, Mouse* mouse);

        static void createMouseButtonTable(lua_State* L, Mouse* mouse);

        static int destroy(lua_State* L);

        static int index(lua_State* L);

        static int isPressed(lua_State* L);
    };
}
