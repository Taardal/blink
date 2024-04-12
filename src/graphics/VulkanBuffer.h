#pragma once

#include "VulkanDevice.h"
#include "VulkanCommandPool.h"

#include <vulkan/vulkan.h>

namespace Blink {

    struct VulkanBufferConfig {
        VulkanDevice* device;
        VulkanCommandPool* commandPool;
        VkDeviceSize size = 0;
        VkBufferUsageFlags usage = 0;
        VkMemoryPropertyFlags memoryProperties = 0;
    };

    class VulkanBuffer {
    private:
        VulkanBufferConfig config;
        VkBuffer buffer = nullptr;
        VkDeviceMemory memory = nullptr;

    public:
        explicit VulkanBuffer(const VulkanBufferConfig& config) noexcept(false);

        ~VulkanBuffer();

        operator VkBuffer() const;

        void setData(void* src) const noexcept(false);

        void copyFrom(VulkanBuffer* sourceBuffer) noexcept(false);

        void copyTo(VulkanBuffer* destinationBuffer) noexcept(false);

    public:
        static void copy(
            VulkanBuffer* sourceBuffer,
            VulkanBuffer* destinationBuffer,
            VulkanDevice* device,
            VulkanCommandPool* commandPool
        ) noexcept(false);
    };
}
