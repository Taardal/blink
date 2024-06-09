#include "CoordinateSystemLuaBinding.h"
#include "lua/GlmLuaBinding.h"
#include "scene/CoordinateSystem.h"

#include <lua.h>

namespace Blink {
    void CoordinateSystemLuaBinding::initialize(lua_State* L) {
        lua_pushvec3(L, POSITIVE_X_AXIS);
        lua_setglobal(L, "POSITIVE_X_AXIS");

        lua_pushvec3(L, POSITIVE_Y_AXIS);
        lua_setglobal(L, "POSITIVE_Y_AXIS");

        lua_pushvec3(L, POSITIVE_Z_AXIS);
        lua_setglobal(L, "POSITIVE_Z_AXIS");

        lua_pushvec3(L, NEGATIVE_X_AXIS);
        lua_setglobal(L, "NEGATIVE_X_AXIS");

        lua_pushvec3(L, NEGATIVE_Y_AXIS);
        lua_setglobal(L, "NEGATIVE_Y_AXIS");

        lua_pushvec3(L, NEGATIVE_Z_AXIS);
        lua_setglobal(L, "NEGATIVE_Z_AXIS");

        lua_pushvec3(L, WORLD_RIGHT_DIRECTION);
        lua_setglobal(L, "WORLD_RIGHT_DIRECTION");

        lua_pushvec3(L, WORLD_UP_DIRECTION);
        lua_setglobal(L, "WORLD_UP_DIRECTION");

        lua_pushvec3(L, WORLD_FORWARD_DIRECTION);
        lua_setglobal(L, "WORLD_FORWARD_DIRECTION");
    }
}
