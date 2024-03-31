#include "VulkanIndexBuffer.h"

namespace Blink {

    VulkanIndexBuffer::VulkanIndexBuffer(const VulkanIndexBufferConfig& config) : config(config) {
    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {
        delete buffer;
    }

    VulkanIndexBuffer::operator VkBuffer() const {
        BL_ASSERT(buffer != nullptr);
        return *buffer;
    }

    bool VulkanIndexBuffer::initialize(const std::vector<uint16_t>& indices) {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VulkanBufferConfig stagingBufferConfig{};
        stagingBufferConfig.size = bufferSize;
        stagingBufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        VulkanBuffer stagingBuffer(config.commandPool, config.device, config.physicalDevice);
        if (!stagingBuffer.initialize(stagingBufferConfig)) {
            BL_LOG_ERROR("Could not initialize staging buffer");
            return false;
        }

        VulkanBufferConfig bufferConfig{};
        bufferConfig.size = sizeof(indices[0]) * indices.size();
        bufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufferConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        buffer = new VulkanBuffer(config.commandPool, config.device, config.physicalDevice);
        if (!buffer->initialize(bufferConfig)) {
            BL_LOG_ERROR("Could not initialize buffer");
            return false;
        }

        stagingBuffer.setData((void*) indices.data());
        stagingBuffer.copyTo(buffer);
        stagingBuffer.terminate();

        return true;
    }

    void VulkanIndexBuffer::terminate() {
        buffer->terminate();
    }

    void VulkanIndexBuffer::bind(VkCommandBuffer commandBuffer) const {
        constexpr VkDeviceSize offset = 0;
        constexpr VkIndexType indexType = VK_INDEX_TYPE_UINT16;
        vkCmdBindIndexBuffer(commandBuffer, *buffer, offset, indexType);
    }

}