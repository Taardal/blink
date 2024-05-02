#include "VulkanShader.h"

namespace Blink {
    VulkanShader::VulkanShader(const VulkanShaderConfig& config) : config(config) {
        VkShaderModuleCreateInfo shaderModuleCreateInfo{};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = config.bytes.size();
        shaderModuleCreateInfo.pCode = (const uint32_t*) config.bytes.data();
        BL_ASSERT_THROW_VK_SUCCESS(config.device->createShaderModule(&shaderModuleCreateInfo, &shaderModule));
    }

    VulkanShader::~VulkanShader() {
        config.device->destroyShaderModule(shaderModule);
    }

    VulkanShader::operator VkShaderModule() const {
        return shaderModule;
    }
}
