#include "GlmLuaBinding.h"
#include "luaUtils.h"
#include "glm/gtx/rotate_vector.hpp"

namespace Blink {
    void GlmLuaBinding::initialize(lua_State* L) {
        std::string typeName = "glm";
        std::string metatableName = typeName + "__meta";

        // Create Lua object
        lua_newtable(L);

        // Create a new metatable and push it onto the Lua stack
        luaL_newmetatable(L, metatableName.c_str());
        // Set the __index metamethod of the metatable to GlmBinding::index
        {
            lua_pushstring(L, "__index");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::index, upvalueCount);
            lua_settable(L, -3);
        }
        // Set the newly created metatable as the metatable of the userdata
        lua_setmetatable(L, -2);

        // Create a global Lua variable and assign the userdata to it (C++ object)
        lua_setglobal(L, typeName.c_str());
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] userdata  Binding object
    int GlmLuaBinding::index(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "cross") {
            lua_pushcfunction(L, GlmLuaBinding::cross);
            return 1;
        }
        if (indexName == "normalize") {
            lua_pushcfunction(L, GlmLuaBinding::normalize);
            return 1;
        }
        if (indexName == "rotateX") {
            lua_pushcfunction(L, GlmLuaBinding::rotateX);
            return 1;
        }
        if (indexName == "rotateY") {
            lua_pushcfunction(L, GlmLuaBinding::rotateY);
            return 1;
        }
        if (indexName == "rotateZ") {
            lua_pushcfunction(L, GlmLuaBinding::rotateZ);
            return 1;
        }
        if (indexName == "vec3") {
            lua_pushcfunction(L, GlmLuaBinding::vec3);
            return 1;
        }
        return 0;
    }

    // Lua stack
    // - [-1] table     Vector A
    // - [-2] table     Vector B
    int GlmLuaBinding::cross(lua_State* L) {
        glm::vec3 vectorA{};
        {
            lua_getfield(L, -1, "x");
            auto x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vectorA.x = x;

            lua_getfield(L, -1, "y");
            auto y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vectorA.y = y;

            lua_getfield(L, -1, "z");
            auto z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vectorA.z = z;
        }
        glm::vec3 vectorB{};
        {
            lua_getfield(L, -2, "x");
            auto x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vectorB.x = x;

            lua_getfield(L, -2, "y");
            auto y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vectorB.y = y;

            lua_getfield(L, -2, "z");
            auto z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vectorB.z = z;
        }

        auto result = glm::cross(vectorA, vectorB);

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");

        return 1;
    }

    // Lua stack
    // - [-1] table     Vector
    // - [-2] userdata  Binding object
    int GlmLuaBinding::normalize(lua_State* L) {
        glm::vec3 vector{};

        lua_getfield(L, -1, "x");
        auto x = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);
        vector.x = x;

        lua_getfield(L, -1, "y");
        auto y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);
        vector.y = y;

        lua_getfield(L, -1, "z");
        auto z = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);
        vector.z = z;

        auto result = glm::normalize(vector);

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");

        return 1;
    }

    // Lua stack
    // - [-1] number    angle
    // - [-2] table     vector
    int GlmLuaBinding::rotateX(lua_State* L) {
        auto angle = (float) lua_tonumber(L, -1);

        glm::vec3 vector{};
        {
            lua_getfield(L, -2, "x");
            auto x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vector.x = x;

            lua_getfield(L, -2, "y");
            auto y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vector.y = y;

            lua_getfield(L, -2, "z");
            auto z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vector.z = z;
        }

        glm::vec3 result = glm::rotateX(vector, angle);

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");

        return 1;
    }

    // Lua stack
    // - [-1] number    angle
    // - [-2] table     vector
    int GlmLuaBinding::rotateY(lua_State* L) {
        auto angle = (float) lua_tonumber(L, -1);

        glm::vec3 vector{};
        {
            lua_getfield(L, -2, "x");
            auto x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vector.x = x;

            lua_getfield(L, -2, "y");
            auto y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vector.y = y;

            lua_getfield(L, -2, "z");
            auto z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vector.z = z;
        }

        glm::vec3 result = glm::rotateY(vector, angle);

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");

        return 1;
    }

    // Lua stack
    // - [-1] number    angle
    // - [-2] table     vector
    int GlmLuaBinding::rotateZ(lua_State* L) {
        auto angle = (float) lua_tonumber(L, -1);

        glm::vec3 vector{};
        {
            lua_getfield(L, -2, "x");
            auto x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vector.x = x;

            lua_getfield(L, -2, "y");
            auto y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vector.y = y;

            lua_getfield(L, -2, "z");
            auto z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            vector.z = z;
        }

        glm::vec3 result = glm::rotateZ(vector, angle);

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");

        return 1;
    }

    // Lua stack
    // - [-1] number    Z
    // - [-2] number    Y
    // - [-3] number    X
    int GlmLuaBinding::vec3(lua_State* L) {
        glm::vec3 vector{};

        auto z = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);
        vector.z = z;

        auto y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);
        vector.y = y;

        auto x = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);
        vector.x = x;

        lua_newtable(L);
        lua_pushnumber(L, vector.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, vector.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, vector.z);
        lua_setfield(L, -2, "z");

        return 1;
    }
}
