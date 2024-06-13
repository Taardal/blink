#pragma once

#include <glm/glm.hpp>

// This app uses a right-handed coordinate system in world space:
//
// World space (right-handed)
// - Right:   Positive X
// - Up:      Positive Y
// - Forward: Negative Z
//
// This app uses GLM's glm::perspective function to create a projection matrix. This function flips the Z-axis,
// changing the coordinate system to be left-handed in clip space:
//
// Clip space (left-handed)
// - Right:   Positive X
// - Up:      Positive Y
// - Forward: Positive Z
//
// This is OK because we still have to flip the Y-axis to align with Vulkan, which uses negative Y for its up-direction,
// changing the coordinate system to be right-handed in clip space:
//
// Vulkan clip space (right-handed)
// - Right:   Positive X
// - Up:      Negative Y <-- Opposite from OpenGL
// - Forward: Positive Z
//

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
