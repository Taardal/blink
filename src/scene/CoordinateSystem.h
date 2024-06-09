#pragma once

#include <glm/glm.hpp>

namespace Blink {
    constexpr glm::vec3 POSITIVE_X_AXIS = {1.0f, 0.0f, 0.0f};
    constexpr glm::vec3 POSITIVE_Y_AXIS = {0.0f, 1.0f, 0.0f};
    constexpr glm::vec3 POSITIVE_Z_AXIS = {0.0f, 0.0f, 1.0f};

    constexpr glm::vec3 NEGATIVE_X_AXIS = {-1.0f, 0.0f, 0.0f};
    constexpr glm::vec3 NEGATIVE_Y_AXIS = {0.0f, -1.0f, 0.0f};
    constexpr glm::vec3 NEGATIVE_Z_AXIS = {0.0f, 0.0f, -1.0f};

    // World space coordinate system (right-handed)
    constexpr glm::vec3 WORLD_RIGHT_DIRECTION = POSITIVE_X_AXIS;
    constexpr glm::vec3 WORLD_UP_DIRECTION = POSITIVE_Y_AXIS;
    constexpr glm::vec3 WORLD_FORWARD_DIRECTION = NEGATIVE_Z_AXIS;
}
