#pragma once

#include "graphics/VulkanImage.h"
#include "graphics/VulkanVertexBuffer.h"
#include "graphics/VulkanIndexBuffer.h"
#include "graphics/VulkanUniformBuffer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Blink {
    struct MeshVertex {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        glm::vec2 textureCoordinate = {0.0f, 0.0f};
        uint32_t textureIndex = 0;

        static VkVertexInputBindingDescription getBindingDescription();

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

        bool operator==(const MeshVertex& other) const;
    };
}

template<>
struct std::hash<Blink::MeshVertex> {
    size_t operator()(Blink::MeshVertex const& vertex) const noexcept;
};

namespace Blink {
    struct Mesh {
        glm::mat4 model = glm::mat4(1.0f);
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;
        std::shared_ptr<VulkanVertexBuffer> vertexBuffer = nullptr;
        std::shared_ptr<VulkanIndexBuffer> indexBuffer = nullptr;
        std::vector<std::shared_ptr<VulkanImage>> textures;
        VkDescriptorSet descriptorSet = nullptr;
    };
}

namespace Blink {
    struct MeshPushConstantData {
        alignas(16) glm::mat4 model;
    };
}