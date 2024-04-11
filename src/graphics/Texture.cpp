#include "Texture.h"

namespace Blink {
    Texture::Texture(const TextureConfig& config) : config(config) {
        BL_ASSERT_THROW(!config.path.empty());

        Image image = config.fileSystem->readImage(config.path.c_str());

        VulkanImageConfig vulkanImageConfig{};
        vulkanImageConfig.device = config.device;
        vulkanImageConfig.commandPool = config.commandPool;
        vulkanImageConfig.width = image.width;
        vulkanImageConfig.height = image.height;
        vulkanImageConfig.format = VK_FORMAT_R8G8B8A8_SRGB;
        vulkanImageConfig.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        vulkanImageConfig.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
        vulkanImageConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        BL_TRY(vulkanImage = new VulkanImage(vulkanImageConfig));
        BL_TRY(vulkanImage->setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL));
        BL_TRY(vulkanImage->setData(image));
        BL_TRY(vulkanImage->setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

        image.free();
    }

    Texture::operator VulkanImage*() const {
        return vulkanImage;
    }

    Texture::operator VkImage() const {
        return vulkanImage->getImage();
    }

    Texture::operator VkImageView() const {
        return vulkanImage->getImageView();
    }
}
