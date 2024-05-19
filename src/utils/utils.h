#pragma once

#include <glm/glm.hpp>
#include <cmath>

#include <numbers>

namespace Blink {
    float clampToZero(float x);

    void clampToZero(glm::vec3* vector);

    template<typename T>
    uint32_t sizeOf(uint32_t count) {
        return count * sizeof(T);
    }
}
