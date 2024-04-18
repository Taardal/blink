#pragma once

#include <glm/glm.hpp>

namespace Blink {
    struct PushConstantData {
        alignas(16) glm::mat4 model;
    };
}