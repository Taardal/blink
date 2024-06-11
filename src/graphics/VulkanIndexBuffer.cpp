#include "VulkanIndexBuffer.h"

namespace Blink {

    VulkanIndexBuffer::VulkanIndexBuffer(const VulkanIndexBufferConfig& config) : config(config) {
        BL_ASSERT_THROW(config.size > 0);

        VulkanBufferConfig bufferConfig{};
        bufferConfig.device = config.device;
        bufferConfig.commandPool = config.commandPool;
        bufferConfig.size = config.size;
        bufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
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

    VulkanIndexBuffer::~VulkanIndexBuffer() {
        delete stagingBuffer;
        delete buffer;
    }

    VulkanIndexBuffer::operator VkBuffer() const {
        return *buffer;
    }

    void VulkanIndexBuffer::setData(void* indices) const {
        stagingBuffer->setData(indices);
        stagingBuffer->copyTo(buffer);
    }

    void VulkanIndexBuffer::bind(VkCommandBuffer commandBuffer) const {
        constexpr VkDeviceSize offset = 0;
        constexpr VkIndexType indexType = VK_INDEX_TYPE_UINT32;
        vkCmdBindIndexBuffer(commandBuffer, *buffer, offset, indexType);
    }

}
