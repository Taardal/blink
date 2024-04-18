#pragma once

#include "graphics/Vertex.h"
#include "graphics/VulkanImage.h"
#include "graphics/VulkanVertexBuffer.h"
#include "graphics/VulkanIndexBuffer.h"
#include "graphics/VulkanUniformBuffer.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Blink {
    struct Model {
        glm::mat4 modelMatrix;
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
        VulkanImage* texture = nullptr;
        VulkanVertexBuffer* vertexBuffer = nullptr;
        VulkanIndexBuffer* indexBuffer = nullptr;
        VulkanUniformBuffer* uniformBuffer = nullptr;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    };
}