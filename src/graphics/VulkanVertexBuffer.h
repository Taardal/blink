#pragma once

#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanBuffer.h"
#include "Vertex.h"

#include <vulkan/vulkan.h>

namespace Blink {

    class VulkanVertexBuffer {
    private:
        VulkanCommandPool* commandPool;
        VulkanDevice* device;
        VulkanPhysicalDevice* physicalDevice;
        VulkanBuffer* buffer = nullptr;

    public:
        VulkanVertexBuffer(VulkanCommandPool* commandPool, VulkanDevice* device, VulkanPhysicalDevice* physicalDevice);

        ~VulkanVertexBuffer();

        bool initialize(const std::vector<Vertex>& vertices);

        void terminate();

        void bind(VkCommandBuffer commandBuffer) const;
    };
}
