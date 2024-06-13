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

    struct MeshPushConstantData {
        alignas(16) glm::mat4 model = glm::mat4(1.0f);
    };

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::shared_ptr<VulkanVertexBuffer> vertexBuffer = nullptr;
        std::shared_ptr<VulkanIndexBuffer> indexBuffer = nullptr;
        std::vector<std::shared_ptr<VulkanImage>> textures;
        VkDescriptorSet descriptorSet = nullptr;
        MeshPushConstantData pushConstantData{};
    };
}