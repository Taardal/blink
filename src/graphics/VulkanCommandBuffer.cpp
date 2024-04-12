#include "pch.h"
#include "VulkanCommandBuffer.h"

#include "VulkanApp.h"

namespace Blink {
    VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer commandBuffer) : commandBuffer(commandBuffer) {
    }

    VulkanCommandBuffer::operator VkCommandBuffer() const {
        return commandBuffer;
    }

    VkCommandBuffer VulkanCommandBuffer::vk() const {
        return commandBuffer;
    }

    VkCommandBuffer* VulkanCommandBuffer::vk_ptr() {
        return &commandBuffer;
    }

    const VkCommandBuffer* VulkanCommandBuffer::vk_ptr() const {
        return &commandBuffer;
    }

    VkResult VulkanCommandBuffer::begin(VkCommandBufferUsageFlags usageFlags) const {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = usageFlags;
        return vkBeginCommandBuffer(commandBuffer, &beginInfo);
    }

    VkResult VulkanCommandBuffer::end() const {
        return vkEndCommandBuffer(commandBuffer);
    }

    void VulkanCommandBuffer::reset(VkCommandBufferResetFlags resetFlags) const {
        vkResetCommandBuffer(commandBuffer, resetFlags);
    }
}
