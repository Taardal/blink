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
        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkMemoryPropertyFlags memoryProperties{};
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkImageUsageFlags usage = 0;
        VkImageAspectFlags aspect = 0;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    class VulkanImage {
    private:
        VulkanImageConfig config;
        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
        VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    public:
        VulkanImage(const VulkanImageConfig& config) noexcept(false);

        ~VulkanImage();

        operator VkImage() const;

        VkImage getImage() const;

        VkImageView getImageView() const;

        void setLayout(VkImageLayout layout) noexcept(false);

        void setData(const Image& image) const noexcept(false);

    private:
        void createImage() noexcept(false);

        void createImageView() noexcept(false);

        void initializeImageMemory() noexcept(false);

        static bool hasStencilComponent(VkFormat format);

        static std::string getLayoutName(VkImageLayout layout);
    };

}
