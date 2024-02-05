#pragma once

#include "VulkanBuffer.h"

#include <glm/glm.hpp>

namespace Blink {

    class VulkanUniformBuffer {
    private:
        VulkanCommandPool* commandPool;
        VulkanDevice* device;
        VulkanPhysicalDevice* physicalDevice;
        VulkanBuffer* buffer = nullptr;

    public:
        VulkanUniformBuffer(VulkanCommandPool* commandPool, VulkanDevice* device, VulkanPhysicalDevice* physicalDevice);

        ~VulkanUniformBuffer();

        VulkanBuffer* getBuffer() const;

        bool initialize(uint32_t bufferSize);

        void terminate();

        void setData(void* data) const;
    };

}
