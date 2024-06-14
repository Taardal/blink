#pragma once

#include "graphics/VulkanImage.h"
#include "graphics/VulkanIndexBuffer.h"
#include "graphics/VulkanVertexBuffer.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct SkyboxVertex {
        glm::vec3 position;

        static VkVertexInputBindingDescription getBindingDescription();

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };
}

namespace Blink {
    struct Skybox {
    public:
        static constexpr uint32_t FACE_COUNT = 6;

    public:
        std::shared_ptr<VulkanImage> image = nullptr;
        VkDescriptorSet descriptorSet = nullptr;
        std::shared_ptr<VulkanVertexBuffer> vertexBuffer = nullptr;
        std::shared_ptr<VulkanIndexBuffer> indexBuffer = nullptr;
        std::vector<SkyboxVertex> vertices;
        std::vector<uint32_t> indices;
    };
}
