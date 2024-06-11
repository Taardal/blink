#include "pch.h"
#include "VulkanVertexBuffer.h"

namespace Blink {

    VulkanVertexBuffer::VulkanVertexBuffer(const VulkanVertexBufferConfig& config) : config(config) {
        BL_ASSERT_THROW(config.size > 0);

        VulkanBufferConfig bufferConfig{};
        bufferConfig.device = config.device;
        bufferConfig.commandPool = config.commandPool;
        bufferConfig.size = config.size;
        bufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        buffer = new VulkanBuffer(bufferConfig);

        VulkanBufferConfig stagingBufferConfig{};
        stagingBufferConfig.device = config.device;
        stagingBufferConfig.commandPool = config.commandPool;
        stagingBufferConfig.size = config.size;
        stagingBufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        stagingBuffer = new VulkanBuffer(stagingBufferConfig);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        delete stagingBuffer;
        delete buffer;
    }

    VulkanVertexBuffer::operator VkBuffer() const {
        return *buffer;
    }

    void VulkanVertexBuffer::setData(void* vertices) const {
        stagingBuffer->setData(vertices);
        stagingBuffer->copyTo(buffer);
    }

    void VulkanVertexBuffer::bind(VkCommandBuffer commandBuffer) const {
        VkBuffer buffers[] = { *buffer };
        VkDeviceSize offsets[] = { 0 };
        constexpr uint32_t firstBinding = 0;
        constexpr uint32_t bindingCount = 1;
        vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, buffers, offsets);
    }
}