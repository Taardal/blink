#include "VulkanShader.h"

namespace Blink {

    VulkanShader::VulkanShader(VulkanDevice* vulkanDevice) : vulkanDevice(vulkanDevice) {
    }

    VkShaderModule VulkanShader::getModule() const {
        return shaderModule;
    }

    bool VulkanShader::initialize(const std::vector<char>& bytes) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = bytes.size();
        createInfo.pCode = (const uint32_t*) bytes.data();
        if (vulkanDevice->createShaderModule(&createInfo, &shaderModule) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create shader module");
            return false;
        }
        return true;
    }

    void VulkanShader::terminate() {
        vulkanDevice->destroyShaderModule(shaderModule);
    }
}
