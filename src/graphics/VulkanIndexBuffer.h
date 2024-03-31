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
    };

    class VulkanIndexBuffer {
    private:
        VulkanIndexBufferConfig config;
        VulkanBuffer* buffer = nullptr;

    public:
        explicit VulkanIndexBuffer(const VulkanIndexBufferConfig& config);

        ~VulkanIndexBuffer();

        operator VkBuffer() const;

        bool initialize(const std::vector<uint16_t>& indices);

        void terminate();

        void bind(VkCommandBuffer commandBuffer) const;
    };

}
