#pragma once

#include <vulkan/vulkan.h>

namespace Blink {

    class VulkanCommandBuffer {
    private:
        VkCommandBuffer commandBuffer = nullptr;

    public:
        VulkanCommandBuffer() = default;

        VulkanCommandBuffer(VkCommandBuffer commandBuffer);

        operator VkCommandBuffer() const;

        VkCommandBuffer vk() const;

        VkCommandBuffer* vk_ptr();

        const VkCommandBuffer* vk_ptr() const;

        VkResult begin(VkCommandBufferUsageFlags usageFlags = 0) const;

        VkResult end() const;

        void reset() const;
    };
}
