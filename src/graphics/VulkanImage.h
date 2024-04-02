#pragma once

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanBuffer.h"
#include "system/FileSystem.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct VulkanImageConfig {
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        VkImageCreateInfo* createInfo{};
        VkMemoryPropertyFlags memoryProperties{};
    };

    class VulkanImage {
    private:
        VulkanImageConfig config;
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkFormat format;

    public:
        VulkanImage(const VulkanImageConfig& config) noexcept(false);

        ~VulkanImage();

        operator VkImage() const;

        void setLayout(VkImageLayout layout) noexcept(false);

        void setData(const Image& image) noexcept(false);

    private:
        bool hasStencilComponent(VkFormat format) const;

        static std::string getLayoutName(VkImageLayout layout);
    };

}
