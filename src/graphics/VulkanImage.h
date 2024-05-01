#pragma once

#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanBuffer.h"
#include "system/FileSystem.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct VulkanImageConfig {
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        VkImage image = nullptr;
        VkImageView imageView = nullptr;
        VkMemoryPropertyFlags memoryProperties{};
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkImageUsageFlags usage = 0;
        VkImageAspectFlags aspect = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        std::string debugName = "";
    };

    class VulkanImage {
    private:
        VulkanImageConfig config;
        VkImage image = nullptr;
        VkImageView imageView = nullptr;
        VkDeviceMemory deviceMemory = nullptr;
        VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    public:
        VulkanImage(const VulkanImageConfig& config);

        ~VulkanImage();

        const std::string& getDebugName() { return config.debugName; };

        operator VkImage() const;

        operator VkImageView() const;

        VkImage getImage() const;

        VkImageView getImageView() const;

        void setLayout(VkImageLayout layout);

        void setData(std::shared_ptr<ImageFile> imageFile) const;

    private:
        void createImage();

        void createImageView();

        void initializeImageMemory();

        static bool hasStencilComponent(VkFormat format);

        static std::string getLayoutName(VkImageLayout layout);
    };

}
