#include "pch.h"
#include "VulkanImage.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"

namespace Blink {
    VulkanImage::VulkanImage(const VulkanImageConfig& config) : config(config), currentLayout(config.layout) {
        if (config.image == nullptr) {
            createImage();
            initializeImageMemory();
        }
        if (config.imageView == nullptr) {
            createImageView();
        }
    }

    VulkanImage::~VulkanImage() {
        if (imageView != nullptr) {
            config.device->destroyImageView(imageView);
        }
        if (image != nullptr) {
            config.device->freeMemory(deviceMemory);
            config.device->destroyImage(image);
        }
    }

    VulkanImage::operator VkImage() const {
        return getImage();
    }

    VulkanImage::operator VkImageView() const {
        return getImageView();
    }

    VkImage VulkanImage::getImage() const {
        return image != nullptr ? image : config.image;
    }

    VkImageView VulkanImage::getImageView() const {
        return imageView != nullptr ? imageView : config.imageView;
    }

    VkImageLayout VulkanImage::getImageLayout() const {
        return currentLayout;
    }

    void VulkanImage::setLayout(VkImageLayout layout) {
        VkImageLayout oldLayout = this->currentLayout;
        VkImageLayout newLayout = layout;
        this->currentLayout = newLayout;

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = getImage();
        barrier.subresourceRange.aspectMask = config.aspect;
        barrier.subresourceRange.layerCount = config.layerCount;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (hasStencilComponent(config.format)) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            BL_THROW("Unsupported layout transition: [" + getLayoutName(oldLayout) + " -> " + getLayoutName(newLayout) + "]");
        }

        VulkanCommandBuffer commandBuffer;
        BL_ASSERT_THROW_VK_SUCCESS(config.commandPool->allocateCommandBuffer(&commandBuffer));
        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));

        constexpr VkDependencyFlags dependencyFlags = 0;
        constexpr uint32_t memoryBarrierCount = 0;
        constexpr VkMemoryBarrier* memoryBarriers = nullptr;
        constexpr uint32_t bufferMemoryBarrierCount = 0;
        constexpr VkBufferMemoryBarrier* bufferMemoryBarriers = nullptr;
        constexpr uint32_t imageMemoryBarrierCount = 1;
        vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage,
                destinationStage,
                dependencyFlags,
                memoryBarrierCount,
                memoryBarriers,
                bufferMemoryBarrierCount,
                bufferMemoryBarriers,
                imageMemoryBarrierCount,
                &barrier
        );

        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.end());

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffer.vk_ptr();

        BL_ASSERT_THROW_VK_SUCCESS(config.device->submitToGraphicsQueue(&submitInfo));
        BL_ASSERT_THROW_VK_SUCCESS(config.device->waitUntilGraphicsQueueIsIdle());

        config.commandPool->freeCommandBuffer(commandBuffer.vk_ptr());
    }

    void VulkanImage::setData(const std::shared_ptr<ImageFile>& imageFile) const {
        VulkanBufferConfig stagingBufferConfig{};
        stagingBufferConfig.device = config.device;
        stagingBufferConfig.commandPool = config.commandPool;
        stagingBufferConfig.size = imageFile->size;
        stagingBufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        VulkanBuffer stagingBuffer(stagingBufferConfig);
        stagingBuffer.setData(imageFile->pixels);

        constexpr uint32_t depth = 1;
        VkExtent3D imageExtent = {
                (uint32_t) imageFile->width,
                (uint32_t) imageFile->height,
                depth
        };

        constexpr int32_t x = 0;
        constexpr int32_t y = 0;
        constexpr int32_t z = 0;
        VkOffset3D imageOffset = {x, y, z};

        VkBufferImageCopy copyRegion{};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;
        copyRegion.imageSubresource.aspectMask = config.aspect;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageOffset = imageOffset;
        copyRegion.imageExtent = imageExtent;

        VulkanCommandBuffer commandBuffer;
        BL_ASSERT_THROW_VK_SUCCESS(config.commandPool->allocateCommandBuffer(&commandBuffer));
        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));

        constexpr uint32_t copyRegionCount = 1;
        vkCmdCopyBufferToImage(
            commandBuffer,
            stagingBuffer,
            getImage(),
            currentLayout,
            copyRegionCount,
            &copyRegion
        );

        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.end());

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffer.vk_ptr();

        BL_ASSERT_THROW_VK_SUCCESS(config.device->submitToGraphicsQueue(&submitInfo));
        BL_ASSERT_THROW_VK_SUCCESS(config.device->waitUntilGraphicsQueueIsIdle());

        config.commandPool->freeCommandBuffer(commandBuffer.vk_ptr());
    }

    void VulkanImage::setData(const std::vector<std::shared_ptr<ImageFile>>& imageFiles) const {
        std::vector<VulkanBuffer*> stagingBuffers;

        VulkanCommandBuffer commandBuffer;
        BL_ASSERT_THROW_VK_SUCCESS(config.commandPool->allocateCommandBuffer(&commandBuffer));
        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));

        for (uint32_t i = 0; i < config.layerCount; i++) {
            auto imageFile = imageFiles[i];

            VulkanBufferConfig stagingBufferConfig{};
            stagingBufferConfig.device = config.device;
            stagingBufferConfig.commandPool = config.commandPool;
            stagingBufferConfig.size = imageFile->size;
            stagingBufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            auto stagingBuffer = new VulkanBuffer(stagingBufferConfig);
            stagingBuffer->setData(imageFile->pixels);
            stagingBuffers.push_back(stagingBuffer);

            VkBufferImageCopy copyRegion{};
            copyRegion.imageExtent.width = imageFile->width;
            copyRegion.imageExtent.height = imageFile->height;
            copyRegion.imageExtent.depth = 1;
            copyRegion.imageSubresource.aspectMask = config.aspect;
            copyRegion.imageSubresource.layerCount = 1;
            copyRegion.imageSubresource.baseArrayLayer = i;
            copyRegion.imageSubresource.mipLevel = 0;
            copyRegion.bufferOffset = 0;

            constexpr uint32_t copyRegionCount = 1;
            vkCmdCopyBufferToImage(
                commandBuffer,
                *stagingBuffer,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                copyRegionCount,
                &copyRegion
            );
        }

        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.end());

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffer.vk_ptr();

        BL_ASSERT_THROW_VK_SUCCESS(config.device->submitToGraphicsQueue(&submitInfo));
        BL_ASSERT_THROW_VK_SUCCESS(config.device->waitUntilGraphicsQueueIsIdle());

        config.commandPool->freeCommandBuffer(commandBuffer.vk_ptr());

        for (auto stagingBuffer : stagingBuffers) {
            delete stagingBuffer;
        }
    }

    void VulkanImage::createImage() {
        BL_ASSERT_THROW(config.width > 0);
        BL_ASSERT_THROW(config.height > 0);

        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width = config.width;
        imageCreateInfo.extent.height = config.height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = config.layerCount;
        imageCreateInfo.format = config.format;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.initialLayout = config.layout;
        imageCreateInfo.usage = config.usage;
        imageCreateInfo.flags = config.flags;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createImage(&imageCreateInfo, &image));
    }

    void VulkanImage::initializeImageMemory() {
        VkMemoryRequirements memoryRequirements = config.device->getImageMemoryRequirements(image);

        VulkanPhysicalDevice* physicalDevice = config.device->getPhysicalDevice();
        uint32_t memoryTypeIndex = physicalDevice->getMemoryTypeIndex(memoryRequirements, config.memoryProperties);

        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->allocateMemory(&memoryAllocateInfo, &deviceMemory));
        BL_ASSERT_THROW_VK_SUCCESS(config.device->bindImageMemory(image, deviceMemory));
    }

    void VulkanImage::createImageView() {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = getImage();
        imageViewCreateInfo.viewType = config.imageViewType;
        imageViewCreateInfo.format = config.format;
        imageViewCreateInfo.subresourceRange.aspectMask = config.aspect;
        imageViewCreateInfo.subresourceRange.layerCount = config.layerCount;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createImageView(&imageViewCreateInfo, &imageView));
    }

    bool VulkanImage::hasStencilComponent(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    std::string VulkanImage::getLayoutName(VkImageLayout layout) {
        switch (layout) {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                return "VK_IMAGE_LAYOUT_UNDEFINED";
            case VK_IMAGE_LAYOUT_GENERAL:
                return "VK_IMAGE_LAYOUT_GENERAL";
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                return "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL";
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                return "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL";
            case VK_IMAGE_LAYOUT_PREINITIALIZED:
                return "VK_IMAGE_LAYOUT_PREINITIALIZED";
            case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                return "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR";
            case VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR:
                return "VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR";
            case VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR:
                return "VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR";
            case VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR:
                return "VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR";
            case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
                return "VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR";
            case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
                return "VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT";
            case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
                return "VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR";
            case VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT:
                return "VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT";
            default:
                return "";
        }
    }
}
