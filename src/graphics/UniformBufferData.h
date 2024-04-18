#pragma once

#include <glm/glm.hpp>

struct UniformBufferData {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 projection;
};