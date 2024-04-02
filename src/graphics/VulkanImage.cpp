#include "pch.h"
#include "VulkanImage.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"

namespace Blink {
    VulkanImage::VulkanImage(const VulkanImageConfig& config) : config(config) {
        this->layout = config.createInfo->initialLayout;
        this->format = config.createInfo->format;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createImage(config.createInfo, &image));

        VkMemoryRequirements memoryRequirements = config.device->getImageMemoryRequirements(image);

        uint32_t memoryTypeIndex = config.physicalDevice->getMemoryType(
                memoryRequirements.memoryTypeBits,
                config.memoryProperties
        );

        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->allocateMemory(&memoryAllocateInfo, &deviceMemory));

        config.device->bindImageMemory(image, deviceMemory);
    }

    VulkanImage::~VulkanImage() {
        config.device->destroyImage(image);
        config.device->freeMemory(deviceMemory);
    }

    VulkanImage::operator VkImage() const {
        return image;
    }

    void VulkanImage::setLayout(VkImageLayout layout) {
        VkImageLayout oldLayout = this->layout;
        VkImageLayout newLayout = layout;
        this->layout = newLayout;

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (hasStencilComponent(format)) {
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
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
                   newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask =
                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            BL_THROW("Unsupported layout transition: [" + getLayoutName(oldLayout) + " -> " + getLayoutName(newLayout) +
                     "]");
        }

        VulkanCommandBuffer commandBuffer;
        BL_ASSERT_THROW_VK_SUCCESS(config.commandPool->allocateCommandBuffer(&commandBuffer));
        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));

        constexpr VkDependencyFlags dependencyFlags = 0;
        constexpr uint32_t memoryBarrierCount = 0;
        constexpr VkMemoryBarrier* memoryBarriers = VK_NULL_HANDLE;
        constexpr uint32_t bufferMemoryBarrierCount = 0;
        constexpr VkBufferMemoryBarrier* bufferMemoryBarriers = VK_NULL_HANDLE;
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

        config.device->submitToGraphicsQueue(&submitInfo);
        config.device->waitUntilGraphicsQueueIsIdle();

        config.commandPool->freeCommandBuffer(commandBuffer.vk_ptr());
    }

    void VulkanImage::setData(const Image& image) {
        VulkanBufferConfig stagingBufferConfig{};
        stagingBufferConfig.size = image.size;
        stagingBufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferConfig.memoryProperties =
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        VulkanBuffer stagingBuffer(config.commandPool, config.device, config.physicalDevice);
        stagingBuffer.initialize(stagingBufferConfig);
        stagingBuffer.setData(image.pixels);

        constexpr uint32_t depth = 1;
        VkExtent3D imageExtent = {
                (uint32_t) image.width,
                (uint32_t) image.height,
                depth
        };

        constexpr int32_t x = 0;
        constexpr int32_t y = 0;
        constexpr int32_t z = 0;
        VkOffset3D imageOffset = {x, y, z};

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = imageOffset;
        region.imageExtent = imageExtent;

        VulkanCommandBuffer commandBuffer;
        BL_ASSERT_THROW_VK_SUCCESS(config.commandPool->allocateCommandBuffer(&commandBuffer));
        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));

        constexpr uint32_t regionCount = 1;
        vkCmdCopyBufferToImage(
                commandBuffer,
                stagingBuffer,
                this->image,
                this->layout,
                regionCount,
                &region
        );

        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.end());

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffer.vk_ptr();

        config.device->submitToGraphicsQueue(&submitInfo);
        config.device->waitUntilGraphicsQueueIsIdle();

        config.commandPool->freeCommandBuffer(commandBuffer.vk_ptr());

        stagingBuffer.terminate();
    }

    bool VulkanImage::hasStencilComponent(VkFormat format) const {
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
