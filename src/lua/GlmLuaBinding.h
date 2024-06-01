#pragma once

#include <lua.hpp>

namespace Blink {
    class GlmLuaBinding {
    public:
        static const std::string TYPE_NAME;
        static const std::string TYPE_METATABLE_NAME;
        static const std::string VEC2_METATABLE_NAME;
        static const std::string VEC3_METATABLE_NAME;
        static const std::string VEC4_METATABLE_NAME;

    public:
        static void initialize(lua_State* L);

    private:
        static int index(lua_State* L);

        static int addVec2(lua_State* L);

        static int addVec3(lua_State* L);

        static int addVec4(lua_State* L);

        static int cross(lua_State* L);

        static int divideVec2(lua_State* L);

        static int divideVec3(lua_State* L);

        static int divideVec4(lua_State* L);

        static int lerp(lua_State* L);

        static int lookAt(lua_State* L);

        static int multiplyVec2(lua_State* L);

        static int multiplyVec3(lua_State* L);

        static int multiplyVec4(lua_State* L);

        static int normalize(lua_State* L);

        static int rotate(lua_State* L);

        static int rotateX(lua_State* L);

        static int rotateY(lua_State* L);

        static int rotateZ(lua_State* L);

        static int slerp(lua_State* L);

        static int subtractVec2(lua_State* L);

        static int subtractVec3(lua_State* L);

        static int subtractVec4(lua_State* L);

        static int toQuat(lua_State* L);

        static int vec3(lua_State* L);
    };
}
