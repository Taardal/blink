#pragma once

#include "graphics/VulkanImage.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanCommandPool.h"
#include "system/FileSystem.h"

namespace Blink {
    struct TextureConfig {
        std::string path = "";
        FileSystem* fileSystem = nullptr;
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
    };

    class Texture {
    private:
        TextureConfig config;
        VulkanImage* vulkanImage = nullptr;

    public:
        explicit Texture(const TextureConfig& config);

        operator VulkanImage*() const;

        operator VkImage() const;

        operator VkImageView() const;
    };
}
