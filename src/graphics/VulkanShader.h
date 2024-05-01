#pragma once

#include "VulkanDevice.h"
#include <vulkan/vulkan.h>
#include <string>

namespace Blink {
    struct VulkanShaderConfig {
        VulkanDevice* device = nullptr;
        std::vector<char> bytes;
    };

    class VulkanShader {
    private:
        VulkanShaderConfig config;
        VkShaderModule shaderModule = nullptr;

    public:
        explicit VulkanShader(const VulkanShaderConfig& config);

        ~VulkanShader();

        operator VkShaderModule() const;
    };
}
