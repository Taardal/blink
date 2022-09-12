#pragma once

#include <numbers>
#include <string>

namespace Blink {
    enum class GLSLType
    {
        None = 0,
        Bool,
        Int,
        Float,
        Vec2,
        Vec3,
        Vec4,
        Mat2,
        Mat3,
        Mat4
    };

    struct VertexAttribute {
        GLSLType glslType = GLSLType::None;
        std::string_view name;
        uint32_t size = 0;
        uint32_t length = 0;
        uint32_t offset = 0;
        bool normalized = false;

        VertexAttribute(GLSLType glslType, std::string_view name);

    private:
        uint32_t getSize();

        uint32_t getLength();
    };
}
