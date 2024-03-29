#include "pch.h"
#include "VulkanVertexBuffer.h"

namespace Blink {

    VulkanVertexBuffer::VulkanVertexBuffer(VulkanCommandPool* commandPool, VulkanDevice* device, VulkanPhysicalDevice* physicalDevice)
            : commandPool(commandPool), device(device), physicalDevice(physicalDevice) {}

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        delete buffer;
    }

    VulkanVertexBuffer::operator VkBuffer() const {
        BL_ASSERT(buffer != nullptr);
        return *buffer;
    }

    bool VulkanVertexBuffer::initialize(const std::vector<Vertex>& vertices) {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VulkanBufferConfig stagingBufferConfig{};
        stagingBufferConfig.size = bufferSize;
        stagingBufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        VulkanBuffer stagingBuffer(commandPool, device, physicalDevice);
        if (!stagingBuffer.initialize(stagingBufferConfig)) {
            BL_LOG_ERROR("Could not initialize staging buffer");
            return false;
        }

        VulkanBufferConfig bufferConfig{};
        bufferConfig.size = sizeof(vertices[0]) * vertices.size();
        bufferConfig.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        buffer = new VulkanBuffer(commandPool, device, physicalDevice);
        if (!buffer->initialize(bufferConfig)) {
            BL_LOG_ERROR("Could not initialize buffer");
            return false;
        }

        stagingBuffer.setData((void*) vertices.data());
        stagingBuffer.copyTo(buffer);
        stagingBuffer.terminate();

        return true;
    }

    void VulkanVertexBuffer::terminate() {
        buffer->terminate();
    }

    void VulkanVertexBuffer::bind(VkCommandBuffer commandBuffer) const {
        VkBuffer buffers[] = {
                *buffer
        };
        VkDeviceSize offsets[] = { 0 };
        constexpr uint32_t firstBinding = 0;
        constexpr uint32_t bindingCount = 1;
        vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, buffers, offsets);
    }
}