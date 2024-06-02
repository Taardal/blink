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
        static const std::string MAT2_METATABLE_NAME;
        static const std::string MAT3_METATABLE_NAME;
        static const std::string MAT4_METATABLE_NAME;

    public:
        static void initialize(lua_State* L);

    private:
        static int index(lua_State* L);

        static int addVec2(lua_State* L);

        static int addVec3(lua_State* L);

        static int addVec4(lua_State* L);

        static int addMat2(lua_State* L);

        static int addMat3(lua_State* L);

        static int addMat4(lua_State* L);

        static int cross(lua_State* L);

        static int divideVec2(lua_State* L);

        static int divideVec3(lua_State* L);

        static int divideVec4(lua_State* L);

        static int divideMat2(lua_State* L);

        static int divideMat3(lua_State* L);

        static int divideMat4(lua_State* L);

        static int inverseQuat(lua_State* L);

        static int inverseMat2(lua_State* L);

        static int inverseMat3(lua_State* L);

        static int inverseMat4(lua_State* L);

        static int lerp(lua_State* L);

        static int lookAt(lua_State* L);

        static int mat2(lua_State* L);

        static int mat3(lua_State* L);

        static int mat4(lua_State* L);

        static int mat3ToQuat(lua_State* L);

        static int mat4ToQuat(lua_State* L);

        static int multiplyVec2(lua_State* L);

        static int multiplyVec3(lua_State* L);

        static int multiplyVec4(lua_State* L);

        static int multiplyMat2(lua_State* L);

        static int multiplyMat3(lua_State* L);

        static int multiplyMat4(lua_State* L);

        static int normalize(lua_State* L);

        static int quatToMat4(lua_State* L);

        static int rotate(lua_State* L);

        static int rotateX(lua_State* L);

        static int rotateY(lua_State* L);

        static int rotateZ(lua_State* L);

        static int slerp(lua_State* L);

        static int subtractVec2(lua_State* L);

        static int subtractVec3(lua_State* L);

        static int subtractVec4(lua_State* L);

        static int subtractMat2(lua_State* L);

        static int subtractMat3(lua_State* L);

        static int subtractMat4(lua_State* L);

        static int translate(lua_State* L);

        static int vec2(lua_State* L);

        static int vec3(lua_State* L);

        static int vec4(lua_State* L);
    };
}

glm::vec2 lua_tovec2(lua_State* L, int index);

void lua_pushvec2(lua_State* L, const glm::vec2& vector);

glm::vec3 lua_tovec3(lua_State* L, int index);

void lua_pushvec3(lua_State* L, const glm::vec3& vector);

glm::vec4 lua_tovec4(lua_State* L, int index);

void lua_pushvec4(lua_State* L, const glm::vec4& vector);

glm::mat2 lua_tomat2(lua_State* L, int index);

void lua_pushmat2(lua_State* L, const glm::mat2& matrix);

glm::mat3 lua_tomat3(lua_State* L, int index);

void lua_pushmat3(lua_State* L, const glm::mat3& matrix);

glm::mat4 lua_tomat4(lua_State* L, int index);

void lua_pushmat4(lua_State* L, const glm::mat4& matrix);

glm::quat lua_toquat(lua_State* L, int index);

void lua_pushquat(lua_State* L, const glm::quat& quaternion);
