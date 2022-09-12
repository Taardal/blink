#pragma once

#include <glm/glm.hpp>

namespace Blink {
    struct Vertex {
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        glm::vec2 textureCoordinate = { 0.0f, 0.0f };
        float textureIndex = 0.0f;
        float tilingFactor = 0.0f;
    };
}
