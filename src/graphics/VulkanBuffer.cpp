#include "pch.h"
#include "VulkanBuffer.h"

namespace Blink {
    VulkanBuffer::VulkanBuffer(const VulkanBufferConfig& config) noexcept(false) : config(config) {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = config.size;
        bufferCreateInfo.usage = config.usage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createBuffer(&bufferCreateInfo, &buffer));

        const VkMemoryRequirements& memoryRequirements = config.device->getBufferMemoryRequirements(buffer);

        VulkanPhysicalDevice* physicalDevice = config.device->getPhysicalDevice();
        uint32_t memoryTypeIndex = physicalDevice->getMemoryTypeIndex(memoryRequirements, config.memoryProperties);

        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        BL_ASSERT_THROW_VK_SUCCESS(config.device->allocateMemory(&memoryAllocateInfo, &memory));
        BL_ASSERT_THROW_VK_SUCCESS(config.device->bindBufferMemory(buffer, memory));
    }

    VulkanBuffer::~VulkanBuffer() {
        config.device->destroyBuffer(buffer);
        config.device->freeMemory(memory);
    }

    VulkanBuffer::operator VkBuffer() const {
        return buffer;
    }

    void VulkanBuffer::setData(void* src) const noexcept(false) {
        void* dst;
        BL_ASSERT_THROW_VK_SUCCESS(config.device->mapMemory(memory, config.size, &dst));
        memcpy(dst, src, config.size);
        config.device->unmapMemory(memory);
    }

    void VulkanBuffer::copyTo(VulkanBuffer* destinationBuffer) noexcept(false) {
        copy(this, destinationBuffer, config.device, config.commandPool);
    }

    void VulkanBuffer::copyFrom(VulkanBuffer* sourceBuffer) noexcept(false) {
        copy(sourceBuffer, this, config.device, config.commandPool);
    }

    void VulkanBuffer::copy(
        VulkanBuffer* sourceBuffer,
        VulkanBuffer* destinationBuffer,
        VulkanDevice* device,
        VulkanCommandPool* commandPool
    ) noexcept(false) {
        BL_ASSERT(sourceBuffer->config.size == destinationBuffer->config.size);

        VulkanCommandBuffer commandBuffer;
        BL_ASSERT_THROW_VK_SUCCESS(commandPool->allocateCommandBuffer(&commandBuffer));
        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));

        VkBufferCopy copyRegion{};
        copyRegion.size = sourceBuffer->config.size;
        constexpr uint32_t regionCount = 1;
        vkCmdCopyBuffer(commandBuffer, sourceBuffer->buffer, destinationBuffer->buffer, regionCount, &copyRegion);

        BL_ASSERT_THROW_VK_SUCCESS(commandBuffer.end());

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffer.vk_ptr();

        BL_ASSERT_THROW_VK_SUCCESS(device->submitToGraphicsQueue(&submitInfo));
        BL_ASSERT_THROW_VK_SUCCESS(device->waitUntilGraphicsQueueIsIdle());

        commandPool->freeCommandBuffer(&commandBuffer);
    }
}
