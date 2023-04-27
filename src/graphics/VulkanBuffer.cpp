#include "pch.h"
#include "VulkanBuffer.h"

namespace Blink {

    VulkanBuffer::VulkanBuffer(const VulkanBufferConfig& config, VulkanDevice* device, VulkanPhysicalDevice* physicalDevice)
            : config(config), device(device), physicalDevice(physicalDevice) {}

    const VkBuffer VulkanBuffer::getBuffer() const {
        return buffer;
    }

    bool VulkanBuffer::initialize() {

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
}