#pragma once

#include "graphics/VulkanImage.h"
#include "graphics/VulkanIndexBuffer.h"
#include "graphics/VulkanVertexBuffer.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct Skybox {
        std::shared_ptr<VulkanImage> image = nullptr;
        VkDescriptorSet descriptorSet = nullptr;
        std::shared_ptr<VulkanVertexBuffer> vertexBuffer = nullptr;
        std::shared_ptr<VulkanIndexBuffer> indexBuffer = nullptr;
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
    };
}
