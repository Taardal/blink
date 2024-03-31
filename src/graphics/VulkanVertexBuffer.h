#pragma once

#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanBuffer.h"
#include "Vertex.h"

#include <vulkan/vulkan.h>

namespace Blink {

    struct VulkanVertexBufferConfig {
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
    };

    class VulkanVertexBuffer {
    private:
        VulkanVertexBufferConfig config;
        VulkanBuffer* buffer = nullptr;

    public:
        explicit VulkanVertexBuffer(const VulkanVertexBufferConfig& config);

        ~VulkanVertexBuffer();

        operator VkBuffer() const;

        bool initialize(const std::vector<Vertex>& vertices);

        void terminate();

        void bind(VkCommandBuffer commandBuffer) const;
    };
}
