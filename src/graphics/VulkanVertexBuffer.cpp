#include "pch.h"
#include "VulkanVertexBuffer.h"

namespace Blink {

    VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice* device, VulkanPhysicalDevice* physicalDevice)
            : device(device), physicalDevice(physicalDevice) {}

    VulkanVertexBuffer::~VulkanVertexBuffer() {
        delete buffer;
    }

    bool VulkanVertexBuffer::initialize(const std::vector<Vertex>& vertices) {

        VulkanBufferConfig bufferConfig{};
        bufferConfig.size = sizeof(vertices[0]) * vertices.size();
        bufferConfig.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        buffer = new VulkanBuffer(bufferConfig, device, physicalDevice);
        if (!buffer->initialize()) {
            BL_LOG_ERROR("Could not initialize buffer");
            return false;
        }

        buffer->setData((void*) &vertices);
        return true;
    }

    void VulkanVertexBuffer::terminate() {
        buffer->terminate();
    }

    void VulkanVertexBuffer::bind(VkCommandBuffer commandBuffer) const {
        VkBuffer buffers[] = { buffer->getBuffer() };
        VkDeviceSize offsets[] = { 0 };
        constexpr uint32_t firstBinding = 0;
        constexpr uint32_t bindingCount = 1;
        vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, buffers, offsets);
    }
}