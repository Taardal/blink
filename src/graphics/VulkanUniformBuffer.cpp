#include "VulkanUniformBuffer.h"

namespace Blink {
    VulkanUniformBuffer::VulkanUniformBuffer(const VulkanUniformBufferConfig& config) {
        BL_ASSERT_THROW(config.size > 0);

        VulkanBufferConfig bufferConfig{};
        bufferConfig.device = config.device;
        bufferConfig.commandPool = config.commandPool;
        bufferConfig.size = config.size;
        bufferConfig.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        buffer = new VulkanBuffer(bufferConfig);
    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {
        delete buffer;
    }

    VulkanUniformBuffer::operator VkBuffer() const {
        return *buffer;
    }

    void VulkanUniformBuffer::setData(void* data) const {
        buffer->setData(data);
    }

}
