#pragma once

#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanBuffer.h"

namespace Blink {

    struct VulkanIndexBufferConfig {
        VulkanPhysicalDevice* physicalDevice = nullptr;
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
        explicit VulkanIndexBuffer(const VulkanIndexBufferConfig& config) noexcept(false);

        ~VulkanIndexBuffer();

        operator VkBuffer() const;

        void setData(const std::vector<uint16_t>& indices) const noexcept(false);

        void bind(VkCommandBuffer commandBuffer) const;
    };

}
