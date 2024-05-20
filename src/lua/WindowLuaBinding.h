#pragma once

#include "window/Window.h"

#include <entt/entt.hpp>
#include <lua.hpp>

namespace Blink {
    class WindowLuaBinding {
    private:
        Window* window;

    private:
        explicit WindowLuaBinding(Window* window);

        ~WindowLuaBinding() = default;

    public:
        static void initialize(lua_State* L, Window* window);

    private:
        static int destroy(lua_State* L);

        static int index(lua_State* L);

        static int getAspectRatio(lua_State* L);
    };
}
