#include "CoordinateSystemLuaBinding.h"
#include "lua/GlmLuaBinding.h"
#include "scene/CoordinateSystem.h"

#include <lua.h>

namespace Blink {
    void CoordinateSystemLuaBinding::initialize(lua_State* L) {
        {
            const glm::vec3& direction = POSITIVE_X_AXIS;
            lua_newtable(L);
            lua_pushnumber(L, direction.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, direction.y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, direction.z);
            lua_setfield(L, -2, "z");
            luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
            lua_setmetatable(L, -2);
            lua_setglobal(L, "POSITIVE_X_AXIS");
        }
        {
            const glm::vec3& direction = POSITIVE_Y_AXIS;
            lua_newtable(L);
            lua_pushnumber(L, direction.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, direction.y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, direction.z);
            lua_setfield(L, -2, "z");
            luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
            lua_setmetatable(L, -2);
            lua_setglobal(L, "POSITIVE_Y_AXIS");
        }
        {
            const glm::vec3& direction = POSITIVE_Z_AXIS;
            lua_newtable(L);
            lua_pushnumber(L, direction.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, direction.y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, direction.z);
            lua_setfield(L, -2, "z");
            luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
            lua_setmetatable(L, -2);
            lua_setglobal(L, "POSITIVE_Z_AXIS");
        }
        {
            const glm::vec3& direction = NEGATIVE_X_AXIS;
            lua_newtable(L);
            lua_pushnumber(L, direction.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, direction.y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, direction.z);
            lua_setfield(L, -2, "z");
            luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
            lua_setmetatable(L, -2);
            lua_setglobal(L, "NEGATIVE_X_AXIS");
        }
        {
            const glm::vec3& direction = NEGATIVE_Y_AXIS;
            lua_newtable(L);
            lua_pushnumber(L, direction.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, direction.y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, direction.z);
            lua_setfield(L, -2, "z");
            luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
            lua_setmetatable(L, -2);
            lua_setglobal(L, "NEGATIVE_Y_AXIS");
        }
        {
            const glm::vec3& direction = NEGATIVE_Z_AXIS;
            lua_newtable(L);
            lua_pushnumber(L, direction.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, direction.y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, direction.z);
            lua_setfield(L, -2, "z");
            luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
            lua_setmetatable(L, -2);
            lua_setglobal(L, "NEGATIVE_Z_AXIS");
        }
        {
            const glm::vec3& direction = WORLD_RIGHT_DIRECTION;
            lua_newtable(L);
            lua_pushnumber(L, direction.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, direction.y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, direction.z);
            lua_setfield(L, -2, "z");
            luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
            lua_setmetatable(L, -2);
            lua_setglobal(L, "WORLD_RIGHT_DIRECTION");
        }
        {
            const glm::vec3& direction = WORLD_UP_DIRECTION;
            lua_newtable(L);
            lua_pushnumber(L, direction.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, direction.y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, direction.z);
            lua_setfield(L, -2, "z");
            luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
            lua_setmetatable(L, -2);
            lua_setglobal(L, "WORLD_UP_DIRECTION");
        }
        {
            const glm::vec3& direction = WORLD_FORWARD_DIRECTION;
            lua_newtable(L);
            lua_pushnumber(L, direction.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, direction.y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, direction.z);
            lua_setfield(L, -2, "z");
            luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
            lua_setmetatable(L, -2);
            lua_setglobal(L, "WORLD_FORWARD_DIRECTION");
        }
    }
}
