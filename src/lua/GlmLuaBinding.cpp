#include "GlmLuaBinding.h"
#include "luaUtils.h"

#include <glm/gtx/rotate_vector.hpp>

namespace Blink {
    const std::string GlmLuaBinding::TYPE_NAME = "glm";
    const std::string GlmLuaBinding::TYPE_METATABLE_NAME = TYPE_NAME + "__meta";
    const std::string GlmLuaBinding::VEC2_METATABLE_NAME = TYPE_NAME + ".vec2__meta";
    const std::string GlmLuaBinding::VEC3_METATABLE_NAME = TYPE_NAME + ".vec3__meta";
    const std::string GlmLuaBinding::VEC4_METATABLE_NAME = TYPE_NAME + ".vec4__meta";

    void GlmLuaBinding::initialize(lua_State* L) {
        // --------------------------------------------------------------------------------------------------------------
        // Binding table + metatable
        // --------------------------------------------------------------------------------------------------------------
        // Create the `glm` table and metatable to create new vectors and call glm functions like `glm::cross` and
        // `glm::normalize` etc.
        // --------------------------------------------------------------------------------------------------------------

        // Create Lua object
        lua_newtable(L);

        // Create a new metatable and push it onto the Lua stack
        luaL_newmetatable(L, TYPE_METATABLE_NAME.c_str());
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
        lua_setglobal(L, TYPE_NAME.c_str());

        // --------------------------------------------------------------------------------------------------------------
        // Vector metatables
        // --------------------------------------------------------------------------------------------------------------
        // Create an extra metatable to be added to vector tables. This should not be used on the binding itself, but
        // rather to the tables that represents glm::vecX objects. This metatable facilitates doing arithmetic
        // operations on the tables like `vecA + vecB`, instead of having to do `glm.add(vecA, vecB)`
        // --------------------------------------------------------------------------------------------------------------

        luaL_newmetatable(L, VEC2_METATABLE_NAME.c_str());
        {
            lua_pushstring(L, "__add");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::addVec2, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__sub");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::subtractVec2, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__mul");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::multiplyVec2, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__div");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::divideVec2, upvalueCount);
            lua_settable(L, -3);
        }

        luaL_newmetatable(L, VEC3_METATABLE_NAME.c_str());
        {
            lua_pushstring(L, "__add");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::addVec3, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__sub");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::subtractVec3, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__mul");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::multiplyVec3, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__div");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::divideVec3, upvalueCount);
            lua_settable(L, -3);
        }

        luaL_newmetatable(L, VEC4_METATABLE_NAME.c_str());
        {
            lua_pushstring(L, "__add");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::addVec4, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__sub");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::subtractVec4, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__mul");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::multiplyVec4, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__div");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::divideVec4, upvalueCount);
            lua_settable(L, -3);
        }
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] userdata  Binding
    int GlmLuaBinding::index(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "addVec2") {
            lua_pushcfunction(L, GlmLuaBinding::addVec2);
            return 1;
        }
        if (indexName == "addVec3") {
            lua_pushcfunction(L, GlmLuaBinding::addVec3);
            return 1;
        }
        if (indexName == "addVec4") {
            lua_pushcfunction(L, GlmLuaBinding::addVec4);
            return 1;
        }
        if (indexName == "cross") {
            lua_pushcfunction(L, GlmLuaBinding::cross);
            return 1;
        }
        if (indexName == "divideVec2") {
            lua_pushcfunction(L, GlmLuaBinding::divideVec2);
            return 1;
        }
        if (indexName == "divideVec3") {
            lua_pushcfunction(L, GlmLuaBinding::divideVec3);
            return 1;
        }
        if (indexName == "divideVec4") {
            lua_pushcfunction(L, GlmLuaBinding::divideVec4);
            return 1;
        }
        if (indexName == "lerp") {
            lua_pushcfunction(L, GlmLuaBinding::lerp);
            return 1;
        }
        if (indexName == "lookAt") {
            lua_pushcfunction(L, GlmLuaBinding::lookAt);
            return 1;
        }
        if (indexName == "multiplyVec2") {
            lua_pushcfunction(L, GlmLuaBinding::multiplyVec2);
            return 1;
        }
        if (indexName == "multiplyVec3") {
            lua_pushcfunction(L, GlmLuaBinding::multiplyVec3);
            return 1;
        }
        if (indexName == "multiplyVec4") {
            lua_pushcfunction(L, GlmLuaBinding::multiplyVec4);
            return 1;
        }
        if (indexName == "normalize") {
            lua_pushcfunction(L, GlmLuaBinding::normalize);
            return 1;
        }
        if (indexName == "rotate") {
            lua_pushcfunction(L, GlmLuaBinding::rotate);
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
        if (indexName == "slerp") {
            lua_pushcfunction(L, GlmLuaBinding::slerp);
            return 1;
        }
        if (indexName == "subtractVec2") {
            lua_pushcfunction(L, GlmLuaBinding::subtractVec2);
            return 1;
        }
        if (indexName == "subtractVec3") {
            lua_pushcfunction(L, GlmLuaBinding::subtractVec3);
            return 1;
        }
        if (indexName == "subtractVec4") {
            lua_pushcfunction(L, GlmLuaBinding::subtractVec4);
            return 1;
        }
        if (indexName == "toQuat") {
            lua_pushcfunction(L, GlmLuaBinding::toQuat);
            return 1;
        }
        if (indexName == "vec3") {
            lua_pushcfunction(L, GlmLuaBinding::vec3);
            return 1;
        }
        BL_LOG_WARN("Could not resolve index [{}]", indexName);
        return 0;
    }

    // Lua stack
    // - [-1] table or number    Vector2 B or scalar B
    // - [-2] table or number    Vector2 A or scalar A
    int GlmLuaBinding::addVec2(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec2 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec2 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec2 result;
        if (aIsVector && bIsVector) {
            result = vectorA + vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA + scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA + vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");

        luaL_getmetatable(L, VEC2_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table or number    Vector3 B or scalar B
    // - [-2] table or number    Vector3 A or scalar A
    int GlmLuaBinding::addVec3(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec3 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "z");
                vectorB.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec3 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "z");
                vectorA.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec3 result;
        if (aIsVector && bIsVector) {
            result = vectorA + vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA + scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA + vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table or number    Vector4 B or scalar B
    // - [-2] table or number    Vector4 A or scalar A
    int GlmLuaBinding::addVec4(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec4 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "z");
                vectorB.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "w");
                vectorB.w = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec4 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "z");
                vectorA.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "w");
                vectorA.w = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec4 result;
        if (aIsVector && bIsVector) {
            result = vectorA + vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA + scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA + vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");
        lua_pushnumber(L, result.w);
        lua_setfield(L, -2, "w");

        luaL_getmetatable(L, VEC4_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table     Vector B
    // - [-2] table     Vector A
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

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

        // Lua stack
    // - [-1] table or number    Vector2 B or scalar B
    // - [-2] table or number    Vector2 A or scalar A
    int GlmLuaBinding::divideVec2(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec2 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec2 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec2 result;
        if (aIsVector && bIsVector) {
            result = vectorA / vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA / scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA / vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");

        luaL_getmetatable(L, VEC2_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table or number    Vector3 B or scalar B
    // - [-2] table or number    Vector3 A or scalar A
    int GlmLuaBinding::divideVec3(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec3 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "z");
                vectorB.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec3 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "z");
                vectorA.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec3 result;
        if (aIsVector && bIsVector) {
            result = vectorA / vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA / scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA / vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table or number    Vector4 B or scalar B
    // - [-2] table or number    Vector4 A or scalar A
    int GlmLuaBinding::divideVec4(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec4 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "z");
                vectorB.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "w");
                vectorB.w = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec4 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "z");
                vectorA.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "w");
                vectorA.w = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec4 result;
        if (aIsVector && bIsVector) {
            result = vectorA / vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA / scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA / vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");
        lua_pushnumber(L, result.w);
        lua_setfield(L, -2, "w");

        luaL_getmetatable(L, VEC4_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] number    timestep
    // - [-2] table     End position vec3
    // - [-3] table     Start position vec3
    int GlmLuaBinding::lerp(lua_State* L) {
        float t = (float) lua_tonumber(L, -1);

        glm::vec3 end{};
        {
            lua_getfield(L, -2, "x");
            end.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -2, "y");
            end.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -2, "z");
            end.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }

        glm::vec3 start{};
        {
            lua_getfield(L, -3, "x");
            start.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -3, "y");
            start.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -3, "z");
            start.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }

        glm::vec3 result = start + t * (end - start);

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table     Up direction vector
    // - [-2] table     Target position vector
    // - [-3] table     Position vector
    int GlmLuaBinding::lookAt(lua_State* L) {
        glm::vec3 upDirection{};
        {
            lua_getfield(L, -1, "x");
            upDirection.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "y");
            upDirection.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "z");
            upDirection.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }
        glm::vec3 targetPosition{};
        {
            lua_getfield(L, -2, "x");
            targetPosition.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -2, "y");
            targetPosition.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -2, "z");
            targetPosition.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }
        glm::vec3 position{};
        {
            lua_getfield(L, -3, "x");
            position.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -3, "y");
            position.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -3, "z");
            position.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }

        glm::mat4 view = glm::lookAt(position, targetPosition, upDirection);

        lua_newtable(L);
        for (uint8_t i = 0; i < 4; ++i) {
            lua_newtable(L);
            lua_pushnumber(L, view[i].x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, view[i].y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, view[i].z);
            lua_setfield(L, -2, "z");
            lua_pushnumber(L, view[i].w);
            lua_setfield(L, -2, "w");
            luaL_getmetatable(L, VEC4_METATABLE_NAME.c_str());
            lua_setmetatable(L, -2);
            lua_seti(L, -2, i + 1); // Lua uses 1-based indexing
        }

        return 1;
    }

        // Lua stack
    // - [-1] table or number    Vector2 B or scalar B
    // - [-2] table or number    Vector2 A or scalar A
    int GlmLuaBinding::multiplyVec2(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec2 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec2 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec2 result;
        if (aIsVector && bIsVector) {
            result = vectorA * vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA * scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA * vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");

        luaL_getmetatable(L, VEC2_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table or number    Vector3 B or scalar B
    // - [-2] table or number    Vector3 A or scalar A
    int GlmLuaBinding::multiplyVec3(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec3 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "z");
                vectorB.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec3 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "z");
                vectorA.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec3 result;
        if (aIsVector && bIsVector) {
            result = vectorA * vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA * scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA * vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table or number    Vector4 B or scalar B
    // - [-2] table or number    Vector4 A or scalar A
    int GlmLuaBinding::multiplyVec4(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec4 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "z");
                vectorB.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "w");
                vectorB.w = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec4 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "z");
                vectorA.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "w");
                vectorA.w = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec4 result;
        if (aIsVector && bIsVector) {
            result = vectorA * vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA * scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA * vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");
        lua_pushnumber(L, result.w);
        lua_setfield(L, -2, "w");

        luaL_getmetatable(L, VEC4_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table     Vector
    // - [-2] userdata  Binding
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

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table     vector
    // - [-2] table     quaternion
    int GlmLuaBinding::rotate(lua_State* L) {
        glm::vec3 vector{};
        {
            lua_getfield(L, -1, "x");
            vector.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "y");
            vector.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "z");
            vector.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }

        glm::quat quaternion{};
        {
            lua_getfield(L, -2, "x");
            quaternion.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -2, "y");
            quaternion.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -2, "z");
            quaternion.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -2, "w");
            quaternion.w = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }

        glm::vec3 rotatedVector = glm::rotate(quaternion, vector);

        lua_pushnumber(L, rotatedVector.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, rotatedVector.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, rotatedVector.z);
        lua_setfield(L, -2, "z");

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

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

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

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

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

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

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] number    timestep
    // - [-2] table     End position vec3
    // - [-3] table     Start position vec3
    int GlmLuaBinding::slerp(lua_State* L) {
        float t = (float) lua_tonumber(L, -1);

        glm::vec3 end{};
        {
            lua_getfield(L, -2, "x");
            end.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -2, "y");
            end.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -2, "z");
            end.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }

        glm::vec3 start{};
        {
            lua_getfield(L, -3, "x");
            start.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -3, "y");
            start.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -3, "z");
            start.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }

        glm::quat result = glm::slerp(start, end, t);

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");
        lua_pushnumber(L, result.w);
        lua_setfield(L, -2, "w");

        return 1;
    }

    // Lua stack
    // - [-1] table or number    Vector2 B or scalar B
    // - [-2] table or number    Vector2 A or scalar A
    int GlmLuaBinding::subtractVec2(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec2 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec2 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec2 result;
        if (aIsVector && bIsVector) {
            result = vectorA - vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA - scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA - vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");

        luaL_getmetatable(L, VEC2_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table or number    Vector3 B or scalar B
    // - [-2] table or number    Vector3 A or scalar A
    int GlmLuaBinding::subtractVec3(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec3 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "z");
                vectorB.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec3 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "z");
                vectorA.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec3 result;
        if (aIsVector && bIsVector) {
            result = vectorA - vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA - scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA - vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table or number    Vector4 B or scalar B
    // - [-2] table or number    Vector4 A or scalar A
    int GlmLuaBinding::subtractVec4(lua_State* L) {
        bool bIsVector = lua_istable(L, -1);
        bool bIsScalar = lua_isnumber(L, -1);

        bool aIsVector = lua_istable(L, -2);
        bool aIsScalar = lua_isnumber(L, -2);

        BL_ASSERT_THROW(bIsVector || bIsScalar);
        BL_ASSERT_THROW(aIsVector || aIsScalar);
        BL_ASSERT_THROW(bIsVector || aIsVector);

        glm::vec4 vectorB;
        float scalarB;
        {
            if (bIsVector) {
                lua_getfield(L, -1, "x");
                vectorB.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "y");
                vectorB.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "z");
                vectorB.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "w");
                vectorB.w = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (bIsScalar) {
                scalarB = (float) lua_tonumber(L, -1);
            }
        }
        glm::vec4 vectorA;
        float scalarA;
        {
            if (aIsVector) {
                lua_getfield(L, -2, "x");
                vectorA.x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "y");
                vectorA.y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "z");
                vectorA.z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -2, "w");
                vectorA.w = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            if (aIsScalar) {
                scalarA = (float) lua_tonumber(L, -2);
            }
        }

        glm::vec4 result;
        if (aIsVector && bIsVector) {
            result = vectorA - vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA - scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA - vectorB;
        }

        lua_newtable(L);
        lua_pushnumber(L, result.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, result.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, result.z);
        lua_setfield(L, -2, "z");
        lua_pushnumber(L, result.w);
        lua_setfield(L, -2, "w");

        luaL_getmetatable(L, VEC4_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }

    // Lua stack
    // - [-1] table    Matrix 4x4
    int GlmLuaBinding::toQuat(lua_State* L) {
        glm::mat4 matrix{};

        for (int i = 0; i < 4; ++i) {
            lua_geti(L, -1, i + 1); // Lua uses 1-based indexing

            lua_getfield(L, -1, "x");
            matrix[i].x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "y");
            matrix[i].y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "z");
            matrix[i].z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "w");
            matrix[i].w = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_pop(L, 1);
        }

        glm::quat quaternion = glm::toQuat(matrix);

        lua_newtable(L);
        lua_pushnumber(L, quaternion.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, quaternion.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, quaternion.z);
        lua_setfield(L, -2, "z");
        lua_pushnumber(L, quaternion.w);
        lua_setfield(L, -2, "w");

        return 1;
    }

    // Lua stack
    //
    // glm.vec()
    // - Empty
    //
    // glm.vec({ x, y, z })
    // - [-1] table
    //
    // glm.vec(x, y, z)
    // - [-1] number    Z
    // - [-2] number    Y
    // - [-3] number    X
    int GlmLuaBinding::vec3(lua_State* L) {
        glm::vec3 vector{0, 0, 0};
        uint32_t argumentCount = lua_gettop(L);

        if (argumentCount == 1) {
            lua_getfield(L, -1, "x");
            vector.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "y");
            vector.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "z");
            vector.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }

        if (argumentCount == 3) {
            vector.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            vector.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            vector.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }

        lua_newtable(L);
        lua_pushnumber(L, vector.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, vector.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, vector.z);
        lua_setfield(L, -2, "z");

        luaL_getmetatable(L, VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);

        return 1;
    }
}
