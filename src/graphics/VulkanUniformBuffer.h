#pragma once

#include "VulkanBuffer.h"

#include <glm/glm.hpp>

namespace Blink {

    struct VulkanUniformBufferConfig {
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        VkDeviceSize size = 0;
    };

    class VulkanUniformBuffer {
    private:
        VulkanUniformBufferConfig config;
        VulkanBuffer* buffer = nullptr;

    public:
        VulkanUniformBuffer(const VulkanUniformBufferConfig& config);

        ~VulkanUniformBuffer();

        operator VkBuffer() const;

        void setData(void* data) const;
    };

}
