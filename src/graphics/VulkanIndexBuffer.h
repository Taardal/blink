#pragma once

#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"

namespace Blink {

    struct VulkanIndexBufferConfig {
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        VkDeviceSize size = 0;
    };

    class VulkanIndexBuffer {
    private:
        VulkanIndexBufferConfig config;
        VulkanBuffer* buffer = nullptr;
        VulkanBuffer* stagingBuffer = nullptr;

    public:
        explicit VulkanIndexBuffer(const VulkanIndexBufferConfig& config);

        ~VulkanIndexBuffer();

        operator VkBuffer() const;

        void setData(const std::vector<uint16_t>& indices) const;

        void bind(VkCommandBuffer commandBuffer) const;
    };

}
