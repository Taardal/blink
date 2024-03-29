#pragma once

#include "graphics/VulkanPhysicalDevice.h"
#include "graphics/VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct VulkanImageConfig {
        uint32_t width;
        uint32_t height;
        uint32_t mipLevels;
        VkFormat format;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkMemoryPropertyFlags memoryProperties;
        VkImageLayout layout;
        VkSampleCountFlagBits sampleCount;
    };

    class VulkanImage {
    private:
        VulkanDevice* device;
        VulkanPhysicalDevice* physicalDevice;
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory deviceMemory = VK_NULL_HANDLE;

    public:
        VulkanImage(VulkanDevice* device, VulkanPhysicalDevice* physicalDevice);

        operator VkImage() const;

        bool initialize(const VulkanImageConfig& config);

        void terminate() const;
    };

}
