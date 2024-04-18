#pragma once

#include "VulkanDevice.h"
#include "VulkanCommandPool.h"

#include <vulkan/vulkan.h>

namespace Blink {

    struct VulkanBufferConfig {
        VulkanDevice* device;
        VulkanCommandPool* commandPool;
        VkDeviceSize size = 0;
        VkBufferUsageFlags usage = 0;
        VkMemoryPropertyFlags memoryProperties = 0;
    };

    class VulkanBuffer {
    private:
        VulkanBufferConfig config;
        VkBuffer buffer = nullptr;
        VkDeviceMemory memory = nullptr;

    public:
        explicit VulkanBuffer(const VulkanBufferConfig& config);

        ~VulkanBuffer();

        operator VkBuffer() const;

        void setData(void* src) const;

        void copyFrom(VulkanBuffer* sourceBuffer);

        void copyTo(VulkanBuffer* destinationBuffer);

    public:
        static void copy(
            VulkanBuffer* sourceBuffer,
            VulkanBuffer* destinationBuffer,
            VulkanDevice* device,
            VulkanCommandPool* commandPool
        );
    };
}
