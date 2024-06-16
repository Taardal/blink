#include "GlmLuaBinding.h"
#include "luaUtils.h"

#include <glm/gtx/rotate_vector.hpp>

namespace Blink {
    const std::string GlmLuaBinding::TYPE_NAME = "glm";
    const std::string GlmLuaBinding::TYPE_METATABLE_NAME = TYPE_NAME + "__meta";
    const std::string GlmLuaBinding::VEC2_METATABLE_NAME = TYPE_NAME + ".vec2__meta";
    const std::string GlmLuaBinding::VEC3_METATABLE_NAME = TYPE_NAME + ".vec3__meta";
    const std::string GlmLuaBinding::VEC4_METATABLE_NAME = TYPE_NAME + ".vec4__meta";
    const std::string GlmLuaBinding::MAT2_METATABLE_NAME = TYPE_NAME + ".mat2__meta";
    const std::string GlmLuaBinding::MAT3_METATABLE_NAME = TYPE_NAME + ".mat3__meta";
    const std::string GlmLuaBinding::MAT4_METATABLE_NAME = TYPE_NAME + ".mat4__meta";
    const std::string GlmLuaBinding::QUAT_METATABLE_NAME = TYPE_NAME + ".quat__meta";

    void GlmLuaBinding::initialize(lua_State* L) {
        // --------------------------------------------------------------------------------------------------------------
        // Binding table + metatable
        // --------------------------------------------------------------------------------------------------------------
        // Create the `glm` table and metatable to create new vectors and call glm functions like `glm::cross` and
        // `glm::normalize` etc.
        // --------------------------------------------------------------------------------------------------------------

        lua_newtable(L);
        luaL_newmetatable(L, TYPE_METATABLE_NAME.c_str());

        lua_pushstring(L, "__index");
        lua_pushcfunction(L, GlmLuaBinding::index);
        lua_settable(L, -3);

        lua_setmetatable(L, -2);
        lua_setglobal(L, TYPE_NAME.c_str());

        // -------------------------------------------------------------------------------------------------------------
        // Mathematical object metatables
        // -------------------------------------------------------------------------------------------------------------
        // Create extra metatables to be added to vector, matrix and similar tables. This should not be used on the
        // binding itself, but rather on the tables that represents glm::vecX, glm::matX objects, or similar.
        // -------------------------------------------------------------------------------------------------------------

        createVec2Metatable(L);
        createVec3Metatable(L);
        createVec4Metatable(L);
        createMat2Metatable(L);
        createMat3Metatable(L);
        createMat4Metatable(L);
        createQuatMetatable(L);
    }

    void GlmLuaBinding::createVec2Metatable(lua_State* L) {
        luaL_newmetatable(L, VEC2_METATABLE_NAME.c_str());

        lua_pushstring(L, "__add");
        lua_pushcfunction(L, GlmLuaBinding::addVec2);
        lua_settable(L, -3);

        lua_pushstring(L, "__div");
        lua_pushcfunction(L, GlmLuaBinding::divideVec2);
        lua_settable(L, -3);

        lua_pushstring(L, "__index");
        lua_pushcfunction(L, GlmLuaBinding::indexVec2);
        lua_settable(L, -3);

        lua_pushstring(L, "__len");
        lua_pushcfunction(L, GlmLuaBinding::lengthVec2);
        lua_settable(L, -3);

        lua_pushstring(L, "__mul");
        lua_pushcfunction(L, GlmLuaBinding::multiplyVec2);
        lua_settable(L, -3);

        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, GlmLuaBinding::subtractVec2);
        lua_settable(L, -3);

        lua_pushstring(L, "__unm");
        lua_pushcfunction(L, GlmLuaBinding::unaryMinusVec2);
        lua_settable(L, -3);

        lua_pushstring(L, "__tostring");
        lua_pushcfunction(L, GlmLuaBinding::toStringVec2);
        lua_settable(L, -3);
    }

    void GlmLuaBinding::createVec3Metatable(lua_State* L) {
        luaL_newmetatable(L, VEC3_METATABLE_NAME.c_str());

        lua_pushstring(L, "__add");
        lua_pushcfunction(L, GlmLuaBinding::addVec3);
        lua_settable(L, -3);

        lua_pushstring(L, "__div");
        lua_pushcfunction(L, GlmLuaBinding::divideVec3);
        lua_settable(L, -3);

        lua_pushstring(L, "__index");
        lua_pushcfunction(L, GlmLuaBinding::indexVec3);
        lua_settable(L, -3);

        lua_pushstring(L, "__len");
        lua_pushcfunction(L, GlmLuaBinding::lengthVec3);
        lua_settable(L, -3);

        lua_pushstring(L, "__mul");
        lua_pushcfunction(L, GlmLuaBinding::multiplyVec3);
        lua_settable(L, -3);

        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, GlmLuaBinding::subtractVec3);
        lua_settable(L, -3);

        lua_pushstring(L, "__unm");
        lua_pushcfunction(L, GlmLuaBinding::unaryMinusVec3);
        lua_settable(L, -3);

        lua_pushstring(L, "__tostring");
        lua_pushcfunction(L, GlmLuaBinding::toStringVec3);
        lua_settable(L, -3);
    }

    void GlmLuaBinding::createVec4Metatable(lua_State* L) {
        luaL_newmetatable(L, VEC4_METATABLE_NAME.c_str());

        lua_pushstring(L, "__add");
        lua_pushcfunction(L, GlmLuaBinding::addVec4);
        lua_settable(L, -3);

        lua_pushstring(L, "__div");
        lua_pushcfunction(L, GlmLuaBinding::divideVec4);
        lua_settable(L, -3);

        lua_pushstring(L, "__index");
        lua_pushcfunction(L, GlmLuaBinding::indexVec4);
        lua_settable(L, -3);

        lua_pushstring(L, "__len");
        lua_pushcfunction(L, GlmLuaBinding::lengthVec4);
        lua_settable(L, -3);

        lua_pushstring(L, "__mul");
        lua_pushcfunction(L, GlmLuaBinding::multiplyVec4);
        lua_settable(L, -3);

        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, GlmLuaBinding::subtractVec4);
        lua_settable(L, -3);

        lua_pushstring(L, "__unm");
        lua_pushcfunction(L, GlmLuaBinding::unaryMinusVec4);
        lua_settable(L, -3);

        lua_pushstring(L, "__tostring");
        lua_pushcfunction(L, GlmLuaBinding::toStringVec4);
        lua_settable(L, -3);
    }

    void GlmLuaBinding::createMat2Metatable(lua_State* L) {
        luaL_newmetatable(L, MAT2_METATABLE_NAME.c_str());

        lua_pushstring(L, "__add");
        lua_pushcfunction(L, GlmLuaBinding::addMat2);
        lua_settable(L, -3);

        lua_pushstring(L, "__div");
        lua_pushcfunction(L, GlmLuaBinding::divideMat2);
        lua_settable(L, -3);

        lua_pushstring(L, "__mul");
        lua_pushcfunction(L, GlmLuaBinding::multiplyMat2);
        lua_settable(L, -3);

        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, GlmLuaBinding::subtractMat2);
        lua_settable(L, -3);
    }

    void GlmLuaBinding::createMat3Metatable(lua_State* L) {
        luaL_newmetatable(L, MAT3_METATABLE_NAME.c_str());

        lua_pushstring(L, "__add");
        lua_pushcfunction(L, GlmLuaBinding::addMat3);
        lua_settable(L, -3);

        lua_pushstring(L, "__div");
        lua_pushcfunction(L, GlmLuaBinding::divideMat3);
        lua_settable(L, -3);

        lua_pushstring(L, "__index");
        lua_pushcfunction(L, GlmLuaBinding::indexMat3);
        lua_settable(L, -3);

        lua_pushstring(L, "__mul");
        lua_pushcfunction(L, GlmLuaBinding::multiplyMat3);
        lua_settable(L, -3);

        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, GlmLuaBinding::subtractMat3);
        lua_settable(L, -3);
    }

    void GlmLuaBinding::createMat4Metatable(lua_State* L) {
        luaL_newmetatable(L, MAT4_METATABLE_NAME.c_str());

        lua_pushstring(L, "__add");
        lua_pushcfunction(L, GlmLuaBinding::addMat4);
        lua_settable(L, -3);

        lua_pushstring(L, "__div");
        lua_pushcfunction(L, GlmLuaBinding::divideMat4);
        lua_settable(L, -3);

        lua_pushstring(L, "__index");
        lua_pushcfunction(L, GlmLuaBinding::indexMat4);
        lua_settable(L, -3);

        lua_pushstring(L, "__mul");
        lua_pushcfunction(L, GlmLuaBinding::multiplyMat4);
        lua_settable(L, -3);

        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, GlmLuaBinding::subtractMat4);
        lua_settable(L, -3);
    }

    void GlmLuaBinding::createQuatMetatable(lua_State* L) {
        luaL_newmetatable(L, QUAT_METATABLE_NAME.c_str());

        lua_pushstring(L, "__add");
        lua_pushcfunction(L, GlmLuaBinding::addQuat);
        lua_settable(L, -3);

        lua_pushstring(L, "__div");
        lua_pushcfunction(L, GlmLuaBinding::divideQuat);
        lua_settable(L, -3);

        lua_pushstring(L, "__index");
        lua_pushcfunction(L, GlmLuaBinding::indexQuat);
        lua_settable(L, -3);

        lua_pushstring(L, "__mul");
        lua_pushcfunction(L, GlmLuaBinding::multiplyQuat);
        lua_settable(L, -3);

        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, GlmLuaBinding::subtractQuat);
        lua_settable(L, -3);
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] userdata  Binding
    int GlmLuaBinding::index(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "addMat2") {
            lua_pushcfunction(L, GlmLuaBinding::addMat2);
            return 1;
        }
        if (indexName == "addMat3") {
            lua_pushcfunction(L, GlmLuaBinding::addMat3);
            return 1;
        }
        if (indexName == "addMat4") {
            lua_pushcfunction(L, GlmLuaBinding::addMat4);
            return 1;
        }
        if (indexName == "addQuat") {
            lua_pushcfunction(L, GlmLuaBinding::addQuat);
            return 1;
        }
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
        if (indexName == "angleAxis") {
            lua_pushcfunction(L, GlmLuaBinding::angleAxis);
            return 1;
        }
        if (indexName == "cross") {
            lua_pushcfunction(L, GlmLuaBinding::cross);
            return 1;
        }
        if (indexName == "degrees") {
            lua_pushcfunction(L, GlmLuaBinding::degrees);
            return 1;
        }
        if (indexName == "divideMat2") {
            lua_pushcfunction(L, GlmLuaBinding::divideMat2);
            return 1;
        }
        if (indexName == "divideMat3") {
            lua_pushcfunction(L, GlmLuaBinding::divideMat3);
            return 1;
        }
        if (indexName == "divideMat4") {
            lua_pushcfunction(L, GlmLuaBinding::divideMat4);
            return 1;
        }
        if (indexName == "divideQuat") {
            lua_pushcfunction(L, GlmLuaBinding::divideQuat);
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
        if (indexName == "dotVec2") {
            lua_pushcfunction(L, GlmLuaBinding::dotVec2);
            return 1;
        }
        if (indexName == "dotVec3") {
            lua_pushcfunction(L, GlmLuaBinding::dotVec3);
            return 1;
        }
        if (indexName == "dotVec4") {
            lua_pushcfunction(L, GlmLuaBinding::dotVec4);
            return 1;
        }
        if (indexName == "eulerAngles") {
            lua_pushcfunction(L, GlmLuaBinding::eulerAngles);
            return 1;
        }
        if (indexName == "inverseQuat") {
            lua_pushcfunction(L, GlmLuaBinding::inverseQuat);
            return 1;
        }
        if (indexName == "inverseMat2") {
            lua_pushcfunction(L, GlmLuaBinding::inverseMat2);
            return 1;
        }
        if (indexName == "inverseMat3") {
            lua_pushcfunction(L, GlmLuaBinding::inverseMat3);
            return 1;
        }
        if (indexName == "inverseMat4") {
            lua_pushcfunction(L, GlmLuaBinding::inverseMat4);
            return 1;
        }
        if (indexName == "lengthVec2") {
            lua_pushcfunction(L, GlmLuaBinding::lengthVec2);
            return 1;
        }
        if (indexName == "lengthVec3") {
            lua_pushcfunction(L, GlmLuaBinding::lengthVec3);
            return 1;
        }
        if (indexName == "lengthVec4") {
            lua_pushcfunction(L, GlmLuaBinding::lengthVec4);
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
        if (indexName == "mat2") {
            lua_pushcfunction(L, GlmLuaBinding::mat2);
            return 1;
        }
        if (indexName == "mat3") {
            lua_pushcfunction(L, GlmLuaBinding::mat3);
            return 1;
        }
        if (indexName == "mat4") {
            lua_pushcfunction(L, GlmLuaBinding::mat4);
            return 1;
        }
        if (indexName == "mat3ToQuat") {
            lua_pushcfunction(L, GlmLuaBinding::mat3ToQuat);
            return 1;
        }
        if (indexName == "mat4ToQuat") {
            lua_pushcfunction(L, GlmLuaBinding::mat4ToQuat);
            return 1;
        }
        if (indexName == "multiplyMat2") {
            lua_pushcfunction(L, GlmLuaBinding::multiplyMat2);
            return 1;
        }
        if (indexName == "multiplyMat3") {
            lua_pushcfunction(L, GlmLuaBinding::multiplyMat3);
            return 1;
        }
        if (indexName == "multiplyMat4") {
            lua_pushcfunction(L, GlmLuaBinding::multiplyMat4);
            return 1;
        }
        if (indexName == "multiplyQuat") {
            lua_pushcfunction(L, GlmLuaBinding::multiplyQuat);
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
        if (indexName == "normalizeVec2") {
            lua_pushcfunction(L, GlmLuaBinding::normalizeVec2);
            return 1;
        }
        if (indexName == "normalizeVec3") {
            lua_pushcfunction(L, GlmLuaBinding::normalizeVec3);
            return 1;
        }
        if (indexName == "normalizeVec4") {
            lua_pushcfunction(L, GlmLuaBinding::normalizeVec4);
            return 1;
        }
        if (indexName == "normalizeQuat") {
            lua_pushcfunction(L, GlmLuaBinding::normalizeQuat);
            return 1;
        }
        if (indexName == "quat") {
            lua_pushcfunction(L, GlmLuaBinding::quat);
            return 1;
        }
        if (indexName == "quatLookAt") {
            lua_pushcfunction(L, GlmLuaBinding::quatLookAt);
            return 1;
        }
        if (indexName == "quatLookAtRH") {
            lua_pushcfunction(L, GlmLuaBinding::quatLookAtRH);
            return 1;
        }
        if (indexName == "quatLookAtLH") {
            lua_pushcfunction(L, GlmLuaBinding::quatLookAtLH);
            return 1;
        }
        if (indexName == "quatToMat4") {
            lua_pushcfunction(L, GlmLuaBinding::quatToMat4);
            return 1;
        }
        if (indexName == "radians") {
            lua_pushcfunction(L, GlmLuaBinding::radians);
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
        if (indexName == "subtractMat2") {
            lua_pushcfunction(L, GlmLuaBinding::subtractMat2);
            return 1;
        }
        if (indexName == "subtractMat3") {
            lua_pushcfunction(L, GlmLuaBinding::subtractMat3);
            return 1;
        }
        if (indexName == "subtractMat4") {
            lua_pushcfunction(L, GlmLuaBinding::subtractMat4);
            return 1;
        }
        if (indexName == "subtractQuat") {
            lua_pushcfunction(L, GlmLuaBinding::subtractQuat);
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
        if (indexName == "translate") {
            lua_pushcfunction(L, GlmLuaBinding::translate);
            return 1;
        }
        if (indexName == "vec2") {
            lua_pushcfunction(L, GlmLuaBinding::vec2);
            return 1;
        }
        if (indexName == "vec3") {
            lua_pushcfunction(L, GlmLuaBinding::vec3);
            return 1;
        }
        if (indexName == "vec4") {
            lua_pushcfunction(L, GlmLuaBinding::vec4);
            return 1;
        }
        if (indexName == "foo") {
            lua_pushcfunction(L, GlmLuaBinding::foo);
            return 1;
        }
        BL_LOG_WARN("Could not resolve index [{}]", indexName);
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] table     Mat3 (self)
    int GlmLuaBinding::indexMat3(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "toQuat") {
            lua_pushcfunction(L, GlmLuaBinding::mat3ToQuat);
            return 1;
        }
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] table     Mat4 (self)
    int GlmLuaBinding::indexMat4(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "toQuat") {
            lua_pushcfunction(L, GlmLuaBinding::mat4ToQuat);
            return 1;
        }
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] table     Quaternion (self)
    int GlmLuaBinding::indexQuat(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "inverse") {
            lua_pushcfunction(L, GlmLuaBinding::inverseQuat);
            return 1;
        }
        if (indexName == "normalize") {
            lua_pushcfunction(L, GlmLuaBinding::normalizeQuat);
            return 1;
        }
        if (indexName == "toMat4") {
            lua_pushcfunction(L, GlmLuaBinding::quatToMat4);
            return 1;
        }
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] table     Vec2 (self)
    int GlmLuaBinding::indexVec2(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "normalize") {
            lua_pushcfunction(L, GlmLuaBinding::normalizeVec2);
            return 1;
        }
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] table     Vec3 (self)
    int GlmLuaBinding::indexVec3(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "normalize") {
            lua_pushcfunction(L, GlmLuaBinding::normalizeVec3);
            return 1;
        }
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] table     Vec4 (self)
    int GlmLuaBinding::indexVec4(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "normalize") {
            lua_pushcfunction(L, GlmLuaBinding::normalizeVec4);
            return 1;
        }
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

        glm::vec2 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec2(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec2 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec2(L, -2);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
        }

        glm::vec2 result{};
        if (aIsVector && bIsVector) {
            result = vectorA + vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA + scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA + vectorB;
        }
        lua_pushvec2(L, result);

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

        glm::vec3 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec3(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec3 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec3(L, -2);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
        }

        glm::vec3 result{};
        if (aIsVector && bIsVector) {
            result = vectorA + vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA + scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA + vectorB;
        }
        lua_pushvec3(L, result);

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

        glm::vec4 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec4(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec4 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec4(L, -1);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
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
        lua_pushvec4(L, result);

        return 1;
    }

    // Lua stack
    // - [-1] table     Mat2 B
    // - [-2] table     Mat2 A
    int GlmLuaBinding::addMat2(lua_State* L) {
        glm::mat2 matrixB = lua_tomat2(L, -1);
        glm::mat2 matrixA = lua_tomat2(L, -2);
        glm::mat2 result = matrixA + matrixB;
        lua_pushmat2(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Mat3 B
    // - [-2] table     Mat3 A
    int GlmLuaBinding::addMat3(lua_State* L) {
        glm::mat3 matrixB = lua_tomat3(L, -1);
        glm::mat3 matrixA = lua_tomat3(L, -2);
        glm::mat3 result = matrixA + matrixB;
        lua_pushmat3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Mat4 B
    // - [-2] table     Mat4 A
    int GlmLuaBinding::addMat4(lua_State* L) {
        glm::mat4 matrixB = lua_tomat4(L, -1);
        glm::mat4 matrixA = lua_tomat4(L, -2);
        glm::mat4 result = matrixA + matrixB;
        lua_pushmat4(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Quat B
    // - [-2] table     Quat A
    int GlmLuaBinding::addQuat(lua_State* L) {
        glm::quat quatB = lua_toquat(L, -1);
        glm::quat quatA = lua_toquat(L, -2);
        glm::quat result = quatA + quatB;
        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Axis vector3
    // - [-2] number    Angle (radians)
    int GlmLuaBinding::angleAxis(lua_State* L) {
        glm::vec3 axis = lua_tovec3(L, -1);
        float angle = (float) lua_tonumber(L, -2);
        glm::quat result = glm::angleAxis(angle, axis);
        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vector B
    // - [-2] table     Vector A
    int GlmLuaBinding::cross(lua_State* L) {
        glm::vec3 vectorA = lua_tovec3(L, -1);
        glm::vec3 vectorB = lua_tovec3(L, -2);
        glm::vec3 result = glm::cross(vectorA, vectorB);
        lua_pushvec3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec3
    int GlmLuaBinding::degrees(lua_State* L) {
        glm::vec3 vector = lua_tovec3(L, -1);
        glm::vec3 result = glm::degrees(vector);
        lua_pushvec3(L, result);
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

        glm::vec2 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec2(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec2 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec2(L, -2);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
        }

        glm::vec2 result{};
        if (aIsVector && bIsVector) {
            result = vectorA / vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA / scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA / vectorB;
        }
        lua_pushvec2(L, result);

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

        glm::vec3 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec3(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec3 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec3(L, -2);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
        }

        glm::vec3 result{};
        if (aIsVector && bIsVector) {
            result = vectorA / vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA / scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA / vectorB;
        }
        lua_pushvec3(L, result);

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

        glm::vec4 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec4(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec4 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec4(L, -1);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
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
        lua_pushvec4(L, result);

        return 1;
    }

    // Lua stack
    // - [-1] table     Mat2 B
    // - [-2] table     Mat2 A
    int GlmLuaBinding::divideMat2(lua_State* L) {
        glm::mat2 matrixB = lua_tomat2(L, -1);
        glm::mat2 matrixA = lua_tomat2(L, -2);
        glm::mat2 result = matrixA / matrixB;
        lua_pushmat2(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Mat3 B
    // - [-2] table     Mat3 A
    int GlmLuaBinding::divideMat3(lua_State* L) {
        glm::mat3 matrixB = lua_tomat3(L, -1);
        glm::mat3 matrixA = lua_tomat3(L, -2);
        glm::mat3 result = matrixA / matrixB;
        lua_pushmat3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Mat4 B
    // - [-2] table     Mat4 A
    int GlmLuaBinding::divideMat4(lua_State* L) {
        glm::mat4 matrixB = lua_tomat4(L, -1);
        glm::mat4 matrixA = lua_tomat4(L, -2);
        glm::mat4 result = matrixA / matrixB;
        lua_pushmat4(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Quat B
    // - [-2] table     Quat A
    int GlmLuaBinding::divideQuat(lua_State* L) {
        glm::quat quatB = lua_toquat(L, -1);
        glm::quat quatA = lua_toquat(L, -2);

        // The division operation quatA / quatB is equivalent to multiplying quatA by the inverse of quatB
        glm::quat result = quatA * glm::inverse(quatB);

        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec2 B
    // - [-2] table     Vec2 A
    int GlmLuaBinding::dotVec2(lua_State* L) {
        glm::vec2 vectorB = lua_tovec2(L, -1);
        glm::vec2 vectorA = lua_tovec2(L, -2);
        float result = glm::dot(vectorA, vectorB);
        lua_pushnumber(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec3 B
    // - [-2] table     Vec3 A
    int GlmLuaBinding::dotVec3(lua_State* L) {
        glm::vec3 vectorB = lua_tovec3(L, -1);
        glm::vec3 vectorA = lua_tovec3(L, -2);
        float result = glm::dot(vectorA, vectorB);
        lua_pushnumber(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec4 B
    // - [-2] table     Vec4 A
    int GlmLuaBinding::dotVec4(lua_State* L) {
        glm::vec4 vectorB = lua_tovec4(L, -1);
        glm::vec4 vectorA = lua_tovec4(L, -2);
        float result = glm::dot(vectorA, vectorB);
        lua_pushnumber(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table    Quaternion
    int GlmLuaBinding::eulerAngles(lua_State* L) {
        glm::quat quaternion = lua_toquat(L, -1);
        glm::vec3 result = glm::eulerAngles(quaternion);
        lua_pushvec3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table    Quaternion
    int GlmLuaBinding::inverseQuat(lua_State* L) {
        glm::quat quaternion = lua_toquat(L, -1);
        glm::quat result = glm::inverse(quaternion);
        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table    Mat2
    int GlmLuaBinding::inverseMat2(lua_State* L) {
        glm::mat2 matrix = lua_tomat2(L, -1);
        glm::mat2 result = glm::inverse(matrix);
        lua_pushmat2(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table    Mat3
    int GlmLuaBinding::inverseMat3(lua_State* L) {
        glm::mat3 matrix = lua_tomat3(L, -1);
        glm::mat3 result = glm::inverse(matrix);
        lua_pushmat3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table    Mat4
    int GlmLuaBinding::inverseMat4(lua_State* L) {
        glm::mat4 matrix = lua_tomat4(L, -1);
        glm::mat4 result = glm::inverse(matrix);
        lua_pushmat4(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec2
    int GlmLuaBinding::lengthVec2(lua_State* L) {
        glm::vec2 vector = lua_tovec2(L, -1);
        float result = glm::length(vector);
        lua_pushnumber(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec3
    int GlmLuaBinding::lengthVec3(lua_State* L) {
        glm::vec3 vector = lua_tovec3(L, -1);
        float result = glm::length(vector);
        lua_pushnumber(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec4
    int GlmLuaBinding::lengthVec4(lua_State* L) {
        glm::vec4 vector = lua_tovec4(L, -1);
        float result = glm::length(vector);
        lua_pushnumber(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] number    timestep
    // - [-2] table     End position vec3
    // - [-3] table     Start position vec3
    int GlmLuaBinding::lerp(lua_State* L) {
        float timestep = (float) lua_tonumber(L, -1);
        glm::vec3 end = lua_tovec3(L, -2);
        glm::vec3 start = lua_tovec3(L, -3);
        glm::vec3 result = glm::mix(start, end, timestep);
        lua_pushvec3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Up direction vec3
    // - [-2] table     Center vec3
    // - [-3] table     Eye vec3
    int GlmLuaBinding::lookAt(lua_State* L) {
        glm::vec3 upDirection = lua_tovec3(L, -1);
        glm::vec3 center = lua_tovec3(L, -2);
        glm::vec3 eye = lua_tovec3(L, -3);
        glm::mat4 result = glm::lookAt(eye, center, upDirection);
        lua_pushmat4(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] number   Scalar
    int GlmLuaBinding::mat2(lua_State* L) {
        float scalar = 0.0f;
        bool missing = lua_isnil(L, -1);
        if (!missing) {
            scalar = (float) lua_tonumber(L, -1);
        }
        glm::mat4 result(scalar);
        lua_pushmat2(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] number   Scalar
    int GlmLuaBinding::mat3(lua_State* L) {
        float scalar = 0.0f;
        bool missing = lua_isnil(L, -1);
        if (!missing) {
            scalar = (float) lua_tonumber(L, -1);
        }
        glm::mat4 result(scalar);
        lua_pushmat3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] number   Scalar
    int GlmLuaBinding::mat4(lua_State* L) {
        float scalar = 0.0f;
        bool missing = lua_isnil(L, -1);
        if (!missing) {
            scalar = (float) lua_tonumber(L, -1);
        }
        glm::mat4 result(scalar);
        lua_pushmat4(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table    Matrix 3x3
    int GlmLuaBinding::mat3ToQuat(lua_State* L) {
        glm::mat3 matrix = lua_tomat3(L, -1);
        glm::quat result = glm::toQuat(matrix);
        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table    Matrix 4x4
    int GlmLuaBinding::mat4ToQuat(lua_State* L) {
        glm::mat4 matrix = lua_tomat4(L, -1);
        glm::quat result = glm::toQuat(matrix);
        lua_pushquat(L, result);
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

        glm::vec2 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec2(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec2 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec2(L, -2);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
        }

        glm::vec2 result{};
        if (aIsVector && bIsVector) {
            result = vectorA * vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA * scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA * vectorB;
        }
        lua_pushvec2(L, result);

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

        glm::vec3 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec3(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec3 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec3(L, -2);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
        }

        glm::vec3 result{};
        if (aIsVector && bIsVector) {
            result = vectorA * vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA * scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA * vectorB;
        }
        lua_pushvec3(L, result);

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

        glm::vec4 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec4(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec4 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec4(L, -1);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
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
        lua_pushvec4(L, result);

        return 1;
    }

    // Lua stack
    // - [-1] table     Mat2 B
    // - [-2] table     Mat2 A
    int GlmLuaBinding::multiplyMat2(lua_State* L) {
        glm::mat2 matrixB = lua_tomat2(L, -1);
        glm::mat2 matrixA = lua_tomat2(L, -2);
        glm::mat2 result = matrixA * matrixB;
        lua_pushmat2(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Mat3 B
    // - [-2] table     Mat3 A
    int GlmLuaBinding::multiplyMat3(lua_State* L) {
        glm::mat3 matrixB = lua_tomat3(L, -1);
        glm::mat3 matrixA = lua_tomat3(L, -2);
        glm::mat3 result = matrixA * matrixB;
        lua_pushmat3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Mat4 B
    // - [-2] table     Mat4 A
    int GlmLuaBinding::multiplyMat4(lua_State* L) {
        glm::mat4 matrixB = lua_tomat4(L, -1);
        glm::mat4 matrixA = lua_tomat4(L, -2);
        glm::mat4 result = matrixA * matrixB;
        lua_pushmat4(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Quat B
    // - [-2] table     Quat A
    int GlmLuaBinding::multiplyQuat(lua_State* L) {
        glm::quat quatB = lua_toquat(L, -1);
        glm::quat quatA = lua_toquat(L, -2);
        glm::quat result = quatA * quatB;
        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Quaternion
    int GlmLuaBinding::normalizeQuat(lua_State* L) {
        glm::quat quaternion = lua_toquat(L, -1);
        glm::quat result = glm::normalize(quaternion);
        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec2
    int GlmLuaBinding::normalizeVec2(lua_State* L) {
        glm::vec2 vector = lua_tovec2(L, -1);
        glm::vec2 result = glm::normalize(vector);
        lua_pushvec2(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec3
    int GlmLuaBinding::normalizeVec3(lua_State* L) {
        glm::vec3 vector = lua_tovec3(L, -1);
        glm::vec3 result = glm::normalize(vector);
        lua_pushvec3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec4
    int GlmLuaBinding::normalizeVec4(lua_State* L) {
        glm::vec4 vector = lua_tovec4(L, -1);
        glm::vec4 result = glm::normalize(vector);
        lua_pushvec4(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] number     Z
    // - [-2] number     Y
    // - [-3] number     X
    // - [-4] number     W
    int GlmLuaBinding::quat(lua_State* L) {
        glm::quat result{};
        result.z = (float) lua_tonumber(L, -1);
        result.y = (float) lua_tonumber(L, -2);
        result.x = (float) lua_tonumber(L, -3);
        result.w = (float) lua_tonumber(L, -4);
        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Up direction vector3
    // - [-2] table     Forward direction vector3
    int GlmLuaBinding::quatLookAt(lua_State* L) {
        glm::vec3 upDirection = lua_tovec3(L, -1);
        glm::vec3 forwardDirection = lua_tovec3(L, -2);
        glm::quat result = glm::quatLookAt(forwardDirection, upDirection);
        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Up direction vector3
    // - [-2] table     Forward direction vector3
    int GlmLuaBinding::quatLookAtRH(lua_State* L) {
        glm::vec3 upDirection = lua_tovec3(L, -1);
        glm::vec3 forwardDirection = lua_tovec3(L, -2);
        glm::quat result = glm::quatLookAtRH(forwardDirection, upDirection);
        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Up direction vector3
    // - [-2] table     Forward direction vector3
    int GlmLuaBinding::quatLookAtLH(lua_State* L) {
        glm::vec3 upDirection = lua_tovec3(L, -1);
        glm::vec3 forwardDirection = lua_tovec3(L, -2);
        glm::quat result = glm::quatLookAtLH(forwardDirection, upDirection);
        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table    Quaternion
    int GlmLuaBinding::quatToMat4(lua_State* L) {
        glm::quat quaternion = lua_toquat(L, -1);
        glm::mat4 result = glm::toMat4(quaternion);
        lua_pushmat4(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] number    Angle (degrees)
    int GlmLuaBinding::radians(lua_State* L) {
        float angle = (float) lua_tonumber(L, -1);
        float result = glm::radians(angle);
        lua_pushnumber(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     vector
    // - [-2] table     quaternion
    int GlmLuaBinding::rotate(lua_State* L) {
        glm::vec3 vector = lua_tovec3(L, -1);
        glm::quat quaternion = lua_toquat(L, -2);
        glm::vec3 result = glm::rotate(quaternion, vector);
        lua_pushvec3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] number    Angle
    // - [-2] table     Vector3
    int GlmLuaBinding::rotateX(lua_State* L) {
        auto angle = (float) lua_tonumber(L, -1);
        glm::vec3 vector = lua_tovec3(L, -2);
        glm::vec3 result = glm::rotateX(vector, angle);
        lua_pushvec3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] number    Angle
    // - [-2] table     Vector3
    int GlmLuaBinding::rotateY(lua_State* L) {
        auto angle = (float) lua_tonumber(L, -1);
        glm::vec3 vector = lua_tovec3(L, -2);
        glm::vec3 result = glm::rotateY(vector, angle);
        lua_pushvec3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] number    Angle
    // - [-2] table     Vector3
    int GlmLuaBinding::rotateZ(lua_State* L) {
        auto angle = (float) lua_tonumber(L, -1);
        glm::vec3 vector = lua_tovec3(L, -2);
        glm::vec3 result = glm::rotateZ(vector, angle);
        lua_pushvec3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] number    Timestep
    // - [-2] table     End quaternion
    // - [-3] table     Start quaternion
    int GlmLuaBinding::slerp(lua_State* L) {
        float timestep = (float) lua_tonumber(L, -1);
        glm::quat end = lua_toquat(L, -2);
        glm::quat start = lua_toquat(L, -3);
        glm::quat result = glm::slerp(start, end, timestep);
        lua_pushquat(L, result);
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

        glm::vec2 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec2(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec2 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec2(L, -2);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
        }

        glm::vec2 result{};
        if (aIsVector && bIsVector) {
            result = vectorA - vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA - scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA - vectorB;
        }
        lua_pushvec2(L, result);

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

        glm::vec3 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec3(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec3 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec3(L, -2);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
        }

        glm::vec3 result{};
        if (aIsVector && bIsVector) {
            result = vectorA - vectorB;
        }
        if (aIsVector && bIsScalar) {
            result = vectorA - scalarB;
        }
        if (aIsScalar && bIsVector) {
            result = scalarA - vectorB;
        }
        lua_pushvec3(L, result);

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

        glm::vec4 vectorB{};
        if (bIsVector) {
            vectorB = lua_tovec4(L, -1);
        }
        float scalarB = 0.0f;
        if (bIsScalar) {
            scalarB = (float) lua_tonumber(L, -1);
        }

        glm::vec4 vectorA{};
        if (aIsVector) {
            vectorA = lua_tovec4(L, -1);
        }
        float scalarA = 0.0f;
        if (aIsScalar) {
            scalarA = (float) lua_tonumber(L, -2);
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
        lua_pushvec4(L, result);

        return 1;
    }

    // Lua stack
    // - [-1] table     Vec2 (self)
    int GlmLuaBinding::toStringVec2(lua_State* L) {
        glm::vec2 self = lua_tovec2(L, -1);
        lua_pushfstring(L, "x: %d, y: %d", self.x, self.y);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec3 (self)
    int GlmLuaBinding::toStringVec3(lua_State* L) {
        glm::vec3 self = lua_tovec3(L, -1);
        lua_pushfstring(L, "x: %d, y: %d, z: %d", self.x, self.y, self.z);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec4 (self)
    int GlmLuaBinding::toStringVec4(lua_State* L) {
        glm::vec4 self = lua_tovec4(L, -1);
        lua_pushfstring(L, "x: %d, y: %d, z: %d, w: %d", self.x, self.y, self.z, self.w);
        return 1;
    }

    // Lua stack
    // - [-1] table     Mat2 B
    // - [-2] table     Mat2 A
    int GlmLuaBinding::subtractMat2(lua_State* L) {
        glm::mat2 matrixB = lua_tomat2(L, -1);
        glm::mat2 matrixA = lua_tomat2(L, -2);
        glm::mat2 result = matrixA - matrixB;
        lua_pushmat2(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Mat3 B
    // - [-2] table     Mat3 A
    int GlmLuaBinding::subtractMat3(lua_State* L) {
        glm::mat3 matrixB = lua_tomat3(L, -1);
        glm::mat3 matrixA = lua_tomat3(L, -2);
        glm::mat3 result = matrixA - matrixB;
        lua_pushmat3(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Mat4 B
    // - [-2] table     Mat4 A
    int GlmLuaBinding::subtractMat4(lua_State* L) {
        glm::mat4 matrixB = lua_tomat4(L, -1);
        glm::mat4 matrixA = lua_tomat4(L, -2);
        glm::mat4 result = matrixA - matrixB;
        lua_pushmat4(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Quat B
    // - [-2] table     Quat A
    int GlmLuaBinding::subtractQuat(lua_State* L) {
        glm::quat quatB = lua_toquat(L, -1);
        glm::quat quatA = lua_toquat(L, -2);
        glm::quat result = quatA - quatB;
        lua_pushquat(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table    Vec3
    // - [-2] table    Mat4
    int GlmLuaBinding::translate(lua_State* L) {
        glm::vec3 vector = lua_tovec3(L, -1);
        glm::mat4 matrix = lua_tomat4(L, -2);
        glm::mat4 result = glm::translate(matrix, vector);
        lua_pushmat4(L, result);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec2 (self)
    int GlmLuaBinding::unaryMinusVec2(lua_State* L) {
        glm::vec2 self = lua_tovec2(L, -1);
        lua_pushvec2(L, -self);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec3 (self)
    int GlmLuaBinding::unaryMinusVec3(lua_State* L) {
        glm::vec3 self = lua_tovec3(L, -1);
        lua_pushvec3(L, -self);
        return 1;
    }

    // Lua stack
    // - [-1] table     Vec4 (self)
    int GlmLuaBinding::unaryMinusVec4(lua_State* L) {
        glm::vec4 self = lua_tovec4(L, -1);
        lua_pushvec4(L, -self);
        return 1;
    }

    // Lua stack
    //
    // glm.vec()
    // - Empty
    //
    // glm.vec({ x, y })
    // - [-1] table
    //
    // glm.vec(x, y)
    // - [-1] number    Y
    // - [-2] number    X
    int GlmLuaBinding::vec2(lua_State* L) {
        glm::vec2 vector{};
        uint32_t argumentCount = lua_gettop(L);
        if (argumentCount == 1) {
            vector = lua_tovec2(L, -1);
        }
        if (argumentCount == 2) {
            vector.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            vector.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }
        lua_pushvec2(L, vector);
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
            vector = lua_tovec3(L, -1);
        }
        if (argumentCount == 3) {
            vector.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            vector.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            vector.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }
        lua_pushvec3(L, vector);
        return 1;
    }

    // Lua stack
    //
    // glm.vec()
    // - Empty
    //
    // glm.vec({ x, y, z, w })
    // - [-1] table
    //
    // glm.vec(x, y, z, w)
    // - [-1] number    W
    // - [-2] number    Z
    // - [-3] number    Y
    // - [-4] number    X
    int GlmLuaBinding::vec4(lua_State* L) {
        glm::vec4 vector{};
        uint32_t argumentCount = lua_gettop(L);
        if (argumentCount == 1) {
            vector = lua_tovec4(L, -1);
        }
        if (argumentCount == 4) {
            vector.w = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            vector.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            vector.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            vector.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }
        lua_pushvec4(L, vector);
        return 1;
    }
}

glm::vec2 lua_tovec2(lua_State* L, int index) {
    glm::vec2 vector{};

    lua_getfield(L, index, "x");
    vector.x = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "y");
    vector.y = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    return vector;
}

void lua_pushvec2(lua_State* L, const glm::vec2& vector) {
    lua_newtable(L);

    lua_pushnumber(L, vector.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, vector.y);
    lua_setfield(L, -2, "y");

    luaL_getmetatable(L, Blink::GlmLuaBinding::VEC2_METATABLE_NAME.c_str());
    lua_setmetatable(L, -2);
}

glm::vec3 lua_tovec3(lua_State* L, int index) {
    glm::vec3 vector{};

    lua_getfield(L, index, "x");
    vector.x = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "y");
    vector.y = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "z");
    vector.z = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    return vector;
}

void lua_pushvec3(lua_State* L, const glm::vec3& vector) {
    lua_newtable(L);

    lua_pushnumber(L, vector.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, vector.y);
    lua_setfield(L, -2, "y");

    lua_pushnumber(L, vector.z);
    lua_setfield(L, -2, "z");

    luaL_getmetatable(L, Blink::GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
    lua_setmetatable(L, -2);
}

glm::vec4 lua_tovec4(lua_State* L, int index) {
    glm::vec4 vector{};

    lua_getfield(L, index, "x");
    vector.x = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "y");
    vector.y = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "z");
    vector.z = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "w");
    vector.w = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    return vector;
}

void lua_pushvec4(lua_State* L, const glm::vec4& vector) {
    lua_newtable(L);

    lua_pushnumber(L, vector.x);
    lua_setfield(L, -2, "x");

    lua_pushnumber(L, vector.y);
    lua_setfield(L, -2, "y");

    lua_pushnumber(L, vector.z);
    lua_setfield(L, -2, "z");

    lua_pushnumber(L, vector.w);
    lua_setfield(L, -2, "w");

    luaL_getmetatable(L, Blink::GlmLuaBinding::VEC4_METATABLE_NAME.c_str());
    lua_setmetatable(L, -2);
}

glm::mat2 lua_tomat2(lua_State* L, int index) {
    glm::mat2 matrix{};
    for (int i = 0; i < 2; ++i) {
        lua_geti(L, index, i + 1); // Lua uses 1-based indexing

        lua_getfield(L, -1, "x");
        matrix[i].x = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "y");
        matrix[i].y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pop(L, 1);
    }
    return matrix;
}

void lua_pushmat2(lua_State* L, const glm::mat2& matrix) {
    lua_newtable(L);
    for (uint8_t i = 0; i < 2; ++i) {
        lua_newtable(L);
        lua_pushnumber(L, matrix[i].x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, matrix[i].y);
        lua_setfield(L, -2, "y");
        luaL_getmetatable(L, Blink::GlmLuaBinding::VEC2_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);
        lua_seti(L, -2, i + 1); // Lua uses 1-based indexing
    }
    luaL_getmetatable(L, Blink::GlmLuaBinding::MAT2_METATABLE_NAME.c_str());
    lua_setmetatable(L, -2);
}

glm::mat3 lua_tomat3(lua_State* L, int index) {
    glm::mat3 matrix{};
    for (int i = 0; i < 3; ++i) {
        lua_geti(L, index, i + 1); // Lua uses 1-based indexing

        lua_getfield(L, -1, "x");
        matrix[i].x = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "y");
        matrix[i].y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "z");
        matrix[i].z = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pop(L, 1);
    }
    return matrix;
}

void lua_pushmat3(lua_State* L, const glm::mat3& matrix) {
    lua_newtable(L);
    for (uint8_t i = 0; i < 3; ++i) {
        lua_newtable(L);
        lua_pushnumber(L, matrix[i].x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, matrix[i].y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, matrix[i].z);
        lua_setfield(L, -2, "z");
        luaL_getmetatable(L, Blink::GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);
        lua_seti(L, -2, i + 1); // Lua uses 1-based indexing
    }
    luaL_getmetatable(L, Blink::GlmLuaBinding::MAT3_METATABLE_NAME.c_str());
    lua_setmetatable(L, -2);
}

glm::mat4 lua_tomat4(lua_State* L, int index) {
    glm::mat4 matrix{};
    for (int i = 0; i < 4; ++i) {
        lua_geti(L, index, i + 1); // Lua uses 1-based indexing

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
    return matrix;
}

void lua_pushmat4(lua_State* L, const glm::mat4& matrix) {
    lua_newtable(L);
    for (uint8_t i = 0; i < 4; ++i) {
        lua_newtable(L);
        lua_pushnumber(L, matrix[i].x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, matrix[i].y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, matrix[i].z);
        lua_setfield(L, -2, "z");
        lua_pushnumber(L, matrix[i].w);
        lua_setfield(L, -2, "w");
        luaL_getmetatable(L, Blink::GlmLuaBinding::VEC4_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);
        lua_seti(L, -2, i + 1); // Lua uses 1-based indexing
    }
    luaL_getmetatable(L, Blink::GlmLuaBinding::MAT4_METATABLE_NAME.c_str());
    lua_setmetatable(L, -2);
}

glm::quat lua_toquat(lua_State* L, int index) {
    glm::quat quaternion{};

    lua_getfield(L, index, "x");
    quaternion.x = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "y");
    quaternion.y = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "z");
    quaternion.z = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "w");
    quaternion.w = (float) lua_tonumber(L, -1);
    lua_pop(L, 1);

    return quaternion;
}

void lua_pushquat(lua_State* L, const glm::quat& quaternion) {
    lua_newtable(L);
    lua_pushnumber(L, quaternion.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, quaternion.y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, quaternion.z);
    lua_setfield(L, -2, "z");
    lua_pushnumber(L, quaternion.w);
    lua_setfield(L, -2, "w");
    luaL_getmetatable(L, Blink::GlmLuaBinding::QUAT_METATABLE_NAME.c_str());
    lua_setmetatable(L, -2);
}
