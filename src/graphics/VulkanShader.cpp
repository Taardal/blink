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
        if (descriptorSetLayout != VK_NULL_HANDLE) {
            device->destroyDescriptorSetLayout(descriptorSetLayout);
        }
    }

    VkDescriptorSetLayout VulkanShader::getLayout() const {
        return descriptorSetLayout;
    }

    void VulkanShader::setLayout(VkDescriptorSetLayout layout) {
        this->descriptorSetLayout = layout;
    }

    void VulkanShader::setLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = bindings.size();
        descriptorSetLayoutCreateInfo.pBindings = bindings.data();
        if (device->createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error(BL_TAG("Could not create descriptor set layout"));
        }
    }
}
