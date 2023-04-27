#pragma once

#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "Vertex.h"

#include <vulkan/vulkan.h>

namespace Blink {

    class VulkanVertexBuffer {
    private:
        VulkanDevice* device;
        VulkanPhysicalDevice* physicalDevice;
        VulkanBuffer* buffer;

    public:
        VulkanVertexBuffer(VulkanDevice* device, VulkanPhysicalDevice* physicalDevice);

        virtual ~VulkanVertexBuffer();

        bool initialize(const std::vector<Vertex>& vertices);

        void terminate();

        void bind(VkCommandBuffer commandBuffer) const;
    };
}
