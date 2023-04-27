#pragma once

#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"

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
        VulkanDevice* device;
        VulkanPhysicalDevice* physicalDevice;
        VkBuffer buffer = nullptr;
        VkDeviceMemory memory = nullptr;

    public:
        VulkanBuffer(const VulkanBufferConfig& config, VulkanDevice* device, VulkanPhysicalDevice* physicalDevice);

        const VkBuffer getBuffer() const;

        bool initialize();

        void terminate();

        void setData(void* src) const;
    };
}
