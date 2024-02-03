#pragma once

#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"

namespace Blink {
    class VulkanCommandPool {
    private:
        VulkanDevice* device;
        VulkanPhysicalDevice* physicalDevice;
        VkCommandPool commandPool = nullptr;

    public:
        VulkanCommandPool(VulkanDevice* device, VulkanPhysicalDevice* physicalDevice);

        bool initialize();

        void terminate();

        bool allocateCommandBuffers(std::vector<VkCommandBuffer>* commandBuffers) const;

        bool allocateCommandBuffers(uint32_t count, VkCommandBuffer* commandBuffers) const;

        void freeCommandBuffer(VkCommandBuffer commandBuffer) const;
    };
}
