#pragma once

#include "VulkanBuffer.h"

#include <glm/glm.hpp>

namespace Blink {

    struct VulkanUniformBufferConfig {
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
    };

    class VulkanUniformBuffer {
    private:
        VulkanUniformBufferConfig config;
        VulkanBuffer* buffer = nullptr;

    public:
        VulkanUniformBuffer(const VulkanUniformBufferConfig& config);

        ~VulkanUniformBuffer();

        operator VkBuffer() const;

        bool initialize(uint32_t bufferSize);

        void terminate();

        void setData(void* data) const;
    };

}
