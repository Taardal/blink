#pragma once

#include <glm/glm.hpp>

#define BL_TO_STRING(x) #x

namespace Blink {
    template<typename T>
    uint32_t sizeOf(uint32_t count) {
        return count * sizeof(T);
    }

    float clampToZero(float x);

    void clampToZero(glm::vec3* vector);
}
