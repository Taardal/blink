#pragma once

#include "Texture.h"
#include <glm/glm.hpp>

namespace Blink {
    struct Quad {
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec2 size = { 0.0f, 0.0f };
        glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        float tilingFactor = 1.0f;
        Texture* texture = nullptr;
    };
}