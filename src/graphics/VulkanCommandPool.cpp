#include "pch.h"
#include "VulkanCommandPool.h"

namespace Blink {
    VulkanCommandPool::VulkanCommandPool(const VulkanCommandPoolConfig& config) : config(config) {
        VulkanPhysicalDevice* physicalDevice = config.device->getPhysicalDevice();
        const QueueFamilyIndices& queueFamilyIndices = physicalDevice->getQueueFamilyIndices();

        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        BL_ASSERT_THROW_VK_SUCCESS(config.device->createCommandPool(&commandPoolCreateInfo, &commandPool));
    }

    VulkanCommandPool::~VulkanCommandPool() {
        config.device->destroyCommandPool(commandPool);
    }

    VulkanCommandPool::operator VkCommandPool() const {
        return commandPool;
    }

    VkResult VulkanCommandPool::allocateCommandBuffers(uint32_t count, VkCommandBuffer* commandBuffers) const {
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = count;
        return config.device->allocateCommandBuffers(&allocateInfo, commandBuffers);
    }

    void VulkanCommandPool::freeCommandBuffers(uint32_t count, VkCommandBuffer* commandBuffers) const {
        config.device->freeCommandBuffers(count, commandBuffers, commandPool);
    }

    VkResult VulkanCommandPool::allocateCommandBuffers(std::vector<VkCommandBuffer>* commandBuffers) const {
        return allocateCommandBuffers(commandBuffers->size(), commandBuffers->data());
    }

    void VulkanCommandPool::freeCommandBuffers(std::vector<VkCommandBuffer>* commandBuffers) const {
        freeCommandBuffers(commandBuffers->size(), commandBuffers->data());
    }

    VkResult VulkanCommandPool::allocateCommandBuffers(std::vector<VulkanCommandBuffer>* commandBuffers) const {
        return allocateCommandBuffers(commandBuffers->size(), (VkCommandBuffer*) commandBuffers->data());
    }

    void VulkanCommandPool::freeCommandBuffers(std::vector<VulkanCommandBuffer>* commandBuffers) const {
        freeCommandBuffers(commandBuffers->size(), (VkCommandBuffer*) commandBuffers->data());
    }

    VkResult VulkanCommandPool::allocateCommandBuffer(VkCommandBuffer* commandBuffer) const {
        return allocateCommandBuffers(1, commandBuffer);
    }

    void VulkanCommandPool::freeCommandBuffer(VkCommandBuffer* commandBuffer) const {
        freeCommandBuffers(1, commandBuffer);
    }

    VkResult VulkanCommandPool::allocateCommandBuffer(VulkanCommandBuffer* commandBuffer) const {
        return allocateCommandBuffers(1, commandBuffer->vk_ptr());
    }

    void VulkanCommandPool::freeCommandBuffer(VulkanCommandBuffer* commandBuffer) const {
        freeCommandBuffers(1, commandBuffer->vk_ptr());
    }
}
