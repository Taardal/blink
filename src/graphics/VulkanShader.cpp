#include "VulkanShader.h"

namespace Blink {
    VulkanShader::VulkanShader(VulkanDevice* device) : device(device) {
    }

    VulkanShader::operator VkShaderModule() const {
        return shaderModule;
    }

    bool VulkanShader::initialize(const std::vector<char>& bytes) {
        VkShaderModuleCreateInfo shaderModuleCreateInfo{};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = bytes.size();
        shaderModuleCreateInfo.pCode = (const uint32_t*) bytes.data();
        if (device->createShaderModule(&shaderModuleCreateInfo, &shaderModule) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create shader module");
            return false;
        }
        return true;
    }

    void VulkanShader::terminate() {
        device->destroyShaderModule(shaderModule);
    }

    VkDescriptorSetLayout VulkanShader::getDescriptorLayout() const {
        return descriptorSetLayout;
    }

    void VulkanShader::setDescriptorLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
        // VkDescriptorSetLayoutBinding uboLayoutBinding{};
        // uboLayoutBinding.binding = 0;
        // uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // uboLayoutBinding.descriptorCount = 1;
        // uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = bindings.size();
        descriptorSetLayoutCreateInfo.pBindings = bindings.data();

        if (device->createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error(BL_TAG("Could not create descriptor set layout"));
        }
    }

    void VulkanShader::createDescriptorSets(uint16_t descriptorSetCount, VkDescriptorPool descriptorPool) {
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(descriptorSetCount, descriptorSetLayout);

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
        descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

        descriptorSets.resize(descriptorSetCount);
        if (device->allocateDescriptorSets(&descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error(BL_TAG("Could not allocate descriptor sets"));
        }
    }

    void VulkanShader::setBufferDescriptors(
        const std::vector<std::tuple<BindingIndex, VkDescriptorType, VkDescriptorBufferInfo>>& bufferDescriptors
    ) const {
        for (uint32_t i = 0; i < bufferDescriptors.size(); i++) {
            // VkDescriptorBufferInfo bufferInfo{};
            // bufferInfo.buffer = uniformBuffers[i]->getBuffer()->getBuffer();
            // bufferInfo.offset = 0;
            // bufferInfo.range = sizeof(UniformBufferObject);

            auto& bufferDescriptor = bufferDescriptors[i];

            uint16_t binding = std::get<uint16_t>(bufferDescriptor);
            VkDescriptorType type = std::get<VkDescriptorType>(bufferDescriptor);
            VkDescriptorBufferInfo bufferInfo = std::get<VkDescriptorBufferInfo>(bufferDescriptor);

            for (size_t j = 0; j < descriptorSets.size(); j++) {
                VkDescriptorSet descriptorSet = descriptorSets[j];

                VkWriteDescriptorSet writeDescriptorSet{};
                writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.dstSet = descriptorSet;
                writeDescriptorSet.dstBinding = binding;
                writeDescriptorSet.descriptorType = type;
                writeDescriptorSet.descriptorCount = 1;
                writeDescriptorSet.pBufferInfo = &bufferInfo;

                device->updateDescriptorSets(1, &writeDescriptorSet);
            }
        }
    }
}
