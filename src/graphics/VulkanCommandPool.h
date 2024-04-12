#pragma once

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"

namespace Blink {
    struct VulkanCommandPoolConfig {
        VulkanDevice* device = nullptr;
    };

    class VulkanCommandPool {
    private:
        VulkanCommandPoolConfig config;
        VkCommandPool commandPool = nullptr;

    public:
        explicit VulkanCommandPool(const VulkanCommandPoolConfig& config);

        ~VulkanCommandPool();

        operator VkCommandPool() const;

        VkResult allocateCommandBuffers(uint32_t count, VkCommandBuffer* commandBuffers) const;

        void freeCommandBuffers(uint32_t count, VkCommandBuffer* commandBuffers) const;

        VkResult allocateCommandBuffers(std::vector<VkCommandBuffer>* commandBuffers) const;

        void freeCommandBuffers(std::vector<VkCommandBuffer>* commandBuffers) const;

        VkResult allocateCommandBuffers(std::vector<VulkanCommandBuffer>* commandBuffers) const;

        void freeCommandBuffers(std::vector<VulkanCommandBuffer>* commandBuffers) const;

        VkResult allocateCommandBuffer(VkCommandBuffer* commandBuffer) const;

        void freeCommandBuffer(VkCommandBuffer* commandBuffer) const;

        VkResult allocateCommandBuffer(VulkanCommandBuffer* commandBuffer) const;

        void freeCommandBuffer(VulkanCommandBuffer* commandBuffer) const;
    };
}
