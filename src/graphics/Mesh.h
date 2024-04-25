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
    struct Mesh {
        glm::mat4 model;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        VulkanVertexBuffer* vertexBuffer = nullptr;
        VulkanIndexBuffer* indexBuffer = nullptr;
        VulkanImage* texture = nullptr;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

        std::vector<VulkanImage*> fooTextures;
    };
}