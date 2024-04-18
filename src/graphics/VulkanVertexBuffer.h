#pragma once

#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "Vertex.h"

#include <vulkan/vulkan.h>

namespace Blink {

    struct VulkanVertexBufferConfig {
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        VkDeviceSize size = 0;
    };

    class VulkanVertexBuffer {
    private:
        VulkanVertexBufferConfig config;
        VulkanBuffer* buffer = nullptr;
        VulkanBuffer* stagingBuffer = nullptr;

    public:
        explicit VulkanVertexBuffer(const VulkanVertexBufferConfig& config);

        ~VulkanVertexBuffer();

        operator VkBuffer() const;

        void setData(const std::vector<Vertex>& vertices) const;

        void bind(VkCommandBuffer commandBuffer) const;
    };
}
