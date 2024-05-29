#include "GlmLuaBinding.h"
#include "luaUtils.h"

#include <glm/gtx/rotate_vector.hpp>

namespace Blink {
    const std::string GlmLuaBinding::TYPE_NAME = "glm";
    const std::string GlmLuaBinding::TYPE_METATABLE_NAME = TYPE_NAME + "__meta";
    const std::string GlmLuaBinding::VEC3_METATABLE_NAME = TYPE_NAME + ".vec3__meta";

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
        // Vector3 metatable
        // --------------------------------------------------------------------------------------------------------------
        // Create an extra metatable to be added to vec3 tables. This should not be used on the binding itself, but
        // rather to the tables that represents glm::vec3 objects. This metatable facilitates doing arithmetic
        // operations on the tables like `vecA + vecB`, instead of having to do `glm.add(vecA, vecB)`
        // --------------------------------------------------------------------------------------------------------------

        luaL_newmetatable(L, VEC3_METATABLE_NAME.c_str());
        {
            lua_pushstring(L, "__add");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::add, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__sub");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::subtract, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__mul");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::multiply, upvalueCount);
            lua_settable(L, -3);
        }
        {
            lua_pushstring(L, "__div");
            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, GlmLuaBinding::divide, upvalueCount);
            lua_settable(L, -3);
        }
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] userdata  Binding
    int GlmLuaBinding::index(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "vec3") {
            lua_pushcfunction(L, GlmLuaBinding::vec3);
            return 1;
        }
        if (indexName == "add") {
            lua_pushcfunction(L, GlmLuaBinding::add);
            return 1;
        }
        if (indexName == "subtract") {
            lua_pushcfunction(L, GlmLuaBinding::subtract);
            return 1;
        }
        if (indexName == "multiply") {
            lua_pushcfunction(L, GlmLuaBinding::multiply);
            return 1;
        }
        if (indexName == "divide") {
            lua_pushcfunction(L, GlmLuaBinding::divide);
            return 1;
        }
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
        BL_LOG_WARN("Could not resolve index [{}]", indexName);
        return 0;
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

    // Lua stack
    // - [-1] table or number    Vector B or scalar B
    // - [-2] table or number    Vector A or scalar A
    int GlmLuaBinding::add(lua_State* L) {
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
                auto x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.x = x;

                lua_getfield(L, -1, "y");
                auto y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.y = y;

                lua_getfield(L, -1, "z");
                auto z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.z = z;
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
                auto x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.x = x;

                lua_getfield(L, -2, "y");
                auto y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.y = y;

                lua_getfield(L, -2, "z");
                auto z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.z = z;
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
    // - [-1] table or number    Vector B or scalar B
    // - [-2] table or number    Vector A or scalar A
    int GlmLuaBinding::subtract(lua_State* L) {
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
                auto x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.x = x;

                lua_getfield(L, -1, "y");
                auto y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.y = y;

                lua_getfield(L, -1, "z");
                auto z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.z = z;
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
                auto x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.x = x;

                lua_getfield(L, -2, "y");
                auto y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.y = y;

                lua_getfield(L, -2, "z");
                auto z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.z = z;
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
    // - [-1] table or number    Vector B or scalar B
    // - [-2] table or number    Vector A or scalar A
    int GlmLuaBinding::multiply(lua_State* L) {
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
                auto x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.x = x;

                lua_getfield(L, -1, "y");
                auto y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.y = y;

                lua_getfield(L, -1, "z");
                auto z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.z = z;
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
                auto x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.x = x;

                lua_getfield(L, -2, "y");
                auto y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.y = y;

                lua_getfield(L, -2, "z");
                auto z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.z = z;
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
    // - [-1] table or number    Vector B or scalar B
    // - [-2] table or number    Vector A or scalar A
    int GlmLuaBinding::divide(lua_State* L) {
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
                auto x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.x = x;

                lua_getfield(L, -1, "y");
                auto y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.y = y;

                lua_getfield(L, -1, "z");
                auto z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorB.z = z;
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
                auto x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.x = x;

                lua_getfield(L, -2, "y");
                auto y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.y = y;

                lua_getfield(L, -2, "z");
                auto z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                vectorA.z = z;
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
}
