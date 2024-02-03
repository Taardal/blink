#pragma once

#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanCommandPool.h"

#include <vulkan/vulkan.h>

namespace Blink {

    struct VulkanBufferConfig {
        VkDeviceSize size = 0;
        VkBufferUsageFlags usage = 0;
        VkMemoryPropertyFlags memoryProperties = 0;
    };

    class VulkanBuffer {
    private:
        VulkanBufferConfig config;
        VulkanCommandPool* commandPool;
        VulkanDevice* device;
        VulkanPhysicalDevice* physicalDevice;
        VkBuffer buffer = nullptr;
        VkDeviceMemory memory = nullptr;

    public:
        VulkanBuffer(const VulkanBufferConfig& config, VulkanCommandPool* commandPool, VulkanDevice* device, VulkanPhysicalDevice* physicalDevice);

        VkBuffer getBuffer() const;

        bool initialize();

        void terminate();

        void setData(void* src) const;

        void copyFrom(VulkanBuffer* sourceBuffer);

        void copyTo(VulkanBuffer* destinationBuffer);

    public:
        static void copy(VulkanBuffer* sourceBuffer, VulkanBuffer* destinationBuffer, VulkanDevice* device, VulkanCommandPool* commandPool);
    };
}
