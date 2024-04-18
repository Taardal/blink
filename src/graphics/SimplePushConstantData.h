#pragma once

#include <glm/glm.hpp>

namespace Blink {
    struct SimplePushConstantData {
        glm::mat4 modelMatrix{1.f};
    };
}