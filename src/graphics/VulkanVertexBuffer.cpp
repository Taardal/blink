#include "pch.h"
#include "VulkanVertexBuffer.h"

namespace Blink {

    VulkanVertexBuffer::VulkanVertexBuffer(const VulkanVertexBufferConfig& config) noexcept(false) : config(config) {
        BL_ASSERT_THROW(config.size > 0);

        VulkanBufferConfig bufferConfig{};
        bufferConfig.physicalDevice = config.physicalDevice;
        bufferConfig.device = config.device;
        bufferConfig.commandPool = config.commandPool;
        bufferConfig.size = config.size;
        bufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        BL_TRY(buffer = new VulkanBuffer(bufferConfig));

        VulkanBufferConfig stagingBufferConfig{};
        stagingBufferConfig.physicalDevice = config.physicalDevice;
        stagingBufferConfig.device = config.device;
        stagingBufferConfig.commandPool = config.commandPool;
        stagingBufferConfig.size = config.size;
        stagingBufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        BL_TRY(stagingBuffer = new VulkanBuffer(stagingBufferConfig));
    }

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        delete stagingBuffer;
        delete buffer;
    }

    VulkanVertexBuffer::operator VkBuffer() const {
        return *buffer;
    }

    void VulkanVertexBuffer::setData(const std::vector<Vertex>& vertices) const noexcept(false) {
        stagingBuffer->setData((void*) vertices.data());
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