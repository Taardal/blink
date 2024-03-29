#include "pch.h"
#include "VulkanImage.h"

namespace Blink {

    VulkanImage::VulkanImage(VulkanDevice* device, VulkanPhysicalDevice* physicalDevice)
            : device(device), physicalDevice(physicalDevice) {}

    VulkanImage::operator VkImage() const {
        return image;
    }

    bool VulkanImage::initialize(const VulkanImageConfig& config) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = config.width;
        imageInfo.extent.height = config.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = config.mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = config.format;
        imageInfo.tiling = config.tiling;
        imageInfo.initialLayout = config.layout;
        imageInfo.usage = config.usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = config.sampleCount;

        if (device->createImage(&imageInfo, &image) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create image");
            return false;
        }

        VkMemoryRequirements memoryRequirements = device->getImageMemoryRequirements(image);

        uint32_t memoryTypeIndex = physicalDevice->getMemoryType(memoryRequirements.memoryTypeBits, config.memoryProperties);

        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        if (device->allocateMemory(&memoryAllocateInfo, &deviceMemory) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not allocate Vulkan image memory");
            return false;
        }

        device->bindImageMemory(image, deviceMemory);

        return true;
    }

    void VulkanImage::terminate() const {
        device->destroyImage(image);
        device->freeMemory(deviceMemory);
    }

}
