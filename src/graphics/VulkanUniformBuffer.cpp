#include "VulkanUniformBuffer.h"

namespace Blink {
    VulkanUniformBuffer::VulkanUniformBuffer(VulkanCommandPool* commandPool, VulkanDevice* device, VulkanPhysicalDevice* physicalDevice)
        : commandPool(commandPool), device(device), physicalDevice(physicalDevice), buffer(new VulkanBuffer(commandPool, device, physicalDevice)) {
    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {
        delete buffer;
    }

    VulkanUniformBuffer::operator VkBuffer() const {
        BL_ASSERT(buffer != nullptr);
        return *buffer;
    }

    bool VulkanUniformBuffer::initialize(uint32_t bufferSize) {
        VulkanBufferConfig bufferConfig{};
        bufferConfig.size = bufferSize;
        bufferConfig.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (!buffer->initialize(bufferConfig)) {
            BL_LOG_ERROR("Could not initialize uniform buffer");
            return false;
        }
        return true;
    }

    void VulkanUniformBuffer::terminate() {
        buffer->terminate();
    }

    void VulkanUniformBuffer::setData(void* data) const {
        buffer->setData(data);
    }

}
