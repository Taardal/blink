#include "pch.h"
#include "VulkanCommandPool.h"

namespace Blink {

    VulkanCommandPool::VulkanCommandPool(
        VulkanDevice* device,
        VulkanPhysicalDevice* physicalDevice
    )
        : device(device),
          physicalDevice(physicalDevice)
    {
        const QueueFamilyIndices& queueFamilyIndices = physicalDevice->getQueueFamilyIndices();

        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (device->createCommandPool(&commandPoolCreateInfo, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("Could not create Vulkan command pool");
        }
    }

    VulkanCommandPool::~VulkanCommandPool() {
        device->destroyCommandPool(commandPool);
    }

    VulkanCommandPool::operator VkCommandPool() const {
        return commandPool;
    }

    bool VulkanCommandPool::allocateCommandBuffers(std::vector<VkCommandBuffer>* commandBuffers) const {
        return allocateCommandBuffers(commandBuffers->size(), commandBuffers->data());
    }

    bool VulkanCommandPool::allocateCommandBuffers(uint32_t count, VkCommandBuffer* commandBuffers) const {
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = count;

        if (device->allocateCommandBuffers(&allocateInfo, commandBuffers) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not allocate [{}] Vulkan command buffers", count);
            return false;
        }
        return true;
    };

    void VulkanCommandPool::freeCommandBuffer(VkCommandBuffer commandBuffer) const {
        device->freeCommandBuffers(1, &commandBuffer, commandPool);
    };
}