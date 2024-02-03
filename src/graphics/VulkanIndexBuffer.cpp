#include "VulkanIndexBuffer.h"

namespace Blink {

    VulkanIndexBuffer::VulkanIndexBuffer(VulkanCommandPool* commandPool, VulkanDevice* device, VulkanPhysicalDevice* physicalDevice)
            : commandPool(commandPool), device(device), physicalDevice(physicalDevice) {}

    VulkanIndexBuffer::~VulkanIndexBuffer() {
        delete buffer;
    }

    bool VulkanIndexBuffer::initialize(const std::vector<uint16_t>& indices) {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VulkanBufferConfig stagingBufferConfig{};
        stagingBufferConfig.size = bufferSize;
        stagingBufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        VulkanBuffer stagingBuffer(stagingBufferConfig, commandPool, device, physicalDevice);
        if (!stagingBuffer.initialize()) {
            BL_LOG_ERROR("Could not initialize staging buffer");
            return false;
        }

        VulkanBufferConfig bufferConfig{};
        bufferConfig.size = sizeof(indices[0]) * indices.size();
        bufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufferConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        buffer = new VulkanBuffer(bufferConfig, commandPool, device, physicalDevice);
        if (!buffer->initialize()) {
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
        vkCmdBindIndexBuffer(commandBuffer, buffer->getBuffer(), offset, indexType);
    }

}