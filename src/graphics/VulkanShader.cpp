#include "VulkanShader.h"

namespace Blink {
    VulkanShader::VulkanShader(const VulkanShaderConfig& config) noexcept(false) : config(config) {
        BL_ASSERT_THROW(config.bytes.size() > 0);

        VkShaderModuleCreateInfo shaderModuleCreateInfo{};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = config.bytes.size();
        shaderModuleCreateInfo.pCode = (const uint32_t*) config.bytes.data();
        BL_ASSERT_THROW_VK_SUCCESS(config.device->createShaderModule(&shaderModuleCreateInfo, &shaderModule));
    }

    VulkanShader::~VulkanShader() {
        config.device->destroyShaderModule(shaderModule);
        if (descriptorSetLayout != nullptr) {
            config.device->destroyDescriptorSetLayout(descriptorSetLayout);
        }
    }

    VulkanShader::operator VkShaderModule() const {
        return shaderModule;
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
        BL_ASSERT_THROW_VK_SUCCESS(config.device->createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, &descriptorSetLayout));
    }
}
