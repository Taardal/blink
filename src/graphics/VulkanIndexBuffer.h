#pragma once

#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanBuffer.h"

namespace Blink {

    class VulkanIndexBuffer {
    private:
        VulkanCommandPool* commandPool;
        VulkanDevice* device;
        VulkanPhysicalDevice* physicalDevice;
        VulkanBuffer* buffer = nullptr;

    public:
        VulkanIndexBuffer(VulkanCommandPool* commandPool, VulkanDevice* device, VulkanPhysicalDevice* physicalDevice);

        ~VulkanIndexBuffer();

        bool initialize(const std::vector<uint16_t>& indices);

        void terminate();

        void bind(VkCommandBuffer commandBuffer) const;
    };

}
