#pragma once

#include <lua.hpp>

namespace Blink {
    class GlmLuaBinding {
    public:
        static void initialize(lua_State* L);

    private:
        static int index(lua_State* L);

        static int cross(lua_State* L);

        static int normalize(lua_State* L);

        static int rotateX(lua_State* L);

        static int rotateY(lua_State* L);

        static int rotateZ(lua_State* L);

        static int vec3(lua_State* L);

        static int multiply(lua_State* L);

        static int add(lua_State* L);
    };
}
