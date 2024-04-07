#pragma once

#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanCommandBuffer.h"

namespace Blink {
    struct VulkanCommandPoolConfig {
        VulkanDevice* device = nullptr;
        VulkanPhysicalDevice* physicalDevice = nullptr;
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

        VkResult allocateCommandBuffers(std::vector<VkCommandBuffer>* commandBuffers) const;

        VkResult allocateCommandBuffer(VkCommandBuffer* commandBuffer) const;

        VkResult allocateCommandBuffer(VulkanCommandBuffer* commandBuffer) const;

        void freeCommandBuffers(uint32_t count, VkCommandBuffer* commandBuffers) const;

        void freeCommandBuffers(std::vector<VkCommandBuffer>* commandBuffers) const;

        void freeCommandBuffer(VkCommandBuffer* commandBuffer) const;

        void freeCommandBuffer(VulkanCommandBuffer* commandBuffer) const;
    };
}
