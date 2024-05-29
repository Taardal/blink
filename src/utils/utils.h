#pragma once

#include <glm/glm.hpp>
#include <cmath>

#include <numbers>

namespace Blink {
    constexpr glm::vec3 POSITIVE_X_AXIS = {1.0f, 0.0f, 0.0f};
    constexpr glm::vec3 POSITIVE_Y_AXIS = {0.0f, 1.0f, 0.0f};
    constexpr glm::vec3 POSITIVE_Z_AXIS = {0.0f, 0.0f, 1.0f};

    constexpr glm::vec3 NEGATIVE_X_AXIS = {-1.0f, 0.0f, 0.0f};
    constexpr glm::vec3 NEGATIVE_Y_AXIS = {0.0f, -1.0f, 0.0f};
    constexpr glm::vec3 NEGATIVE_Z_AXIS = {0.0f, 0.0f, -1.0f};

    template<typename T>
    uint32_t sizeOf(uint32_t count) {
        return count * sizeof(T);
    }

    float clampToZero(float x);

    void clampToZero(glm::vec3* vector);
}
