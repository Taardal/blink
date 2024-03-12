#include "pch.h"
#include "VulkanBuffer.h"

namespace Blink {

    VulkanBuffer::VulkanBuffer(VulkanCommandPool* commandPool, VulkanDevice* device, VulkanPhysicalDevice* physicalDevice)
            : commandPool(commandPool), device(device), physicalDevice(physicalDevice) {}

    VkBuffer VulkanBuffer::getBuffer() const {
        return buffer;
    }

    bool VulkanBuffer::initialize(const VulkanBufferConfig& config) {
        this->config = config;

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = config.size;
        bufferCreateInfo.usage = config.usage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (device->createBuffer(&bufferCreateInfo, &buffer) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create vertex buffer");
            return false;
        }

        const VkMemoryRequirements& memoryRequirements = device->getMemoryRequirements(buffer);

        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = physicalDevice->getMemoryType(memoryRequirements.memoryTypeBits, config.memoryProperties);

        if (device->allocateMemory(&memoryAllocateInfo, &memory) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not allocate vertex buffer memory");
            return false;
        }
        device->bindBufferMemory(buffer, memory);
        return true;
    }

    void VulkanBuffer::terminate() {
        device->destroyBuffer(buffer);
        device->freeMemory(memory);
    }

    void VulkanBuffer::setData(void* src) const {
        void* dst;
        device->mapMemory(memory, config.size, &dst);
        memcpy(dst, src, (size_t) config.size);
        device->unmapMemory(memory);
    }

    void VulkanBuffer::copyTo(VulkanBuffer* destinationBuffer) {
        copy(this, destinationBuffer, device, commandPool);
    }

    void VulkanBuffer::copyFrom(VulkanBuffer* sourceBuffer) {
        copy(sourceBuffer, this, device, commandPool);
    }

    void VulkanBuffer::copy(
            VulkanBuffer* sourceBuffer,
            VulkanBuffer* destinationBuffer,
            VulkanDevice* device,
            VulkanCommandPool* commandPool
    ) {
        BL_ASSERT(sourceBuffer->config.size == destinationBuffer->config.size);

        VkCommandBuffer commandBuffer;
        commandPool->allocateCommandBuffers(1, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = sourceBuffer->config.size;
        constexpr uint32_t regionCount = 1;
        vkCmdCopyBuffer(commandBuffer, sourceBuffer->buffer, destinationBuffer->buffer, regionCount, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        constexpr uint32_t submitCount = 1;
        VkFence fence = VK_NULL_HANDLE;
        vkQueueSubmit(device->getGraphicsQueue(), submitCount, &submitInfo, fence);
        vkQueueWaitIdle(device->getGraphicsQueue());

        commandPool->freeCommandBuffer(commandBuffer);
    }
}