#pragma once

#include <glm/glm.hpp>
#include <cmath>

#include <numbers>

namespace Blink {
    constexpr glm::vec3 X_AXIS = {1.0f, 0.0f, 0.0f};
    constexpr glm::vec3 Y_AXIS = {0.0f, 1.0f, 0.0f};
    constexpr glm::vec3 Z_AXIS = {0.0f, 0.0f, 1.0f};

    float clampToZero(float x);

    void clampToZero(glm::vec3* vector);

    template<typename T>
    uint32_t sizeOf(uint32_t count) {
        return count * sizeof(T);
    }
}
