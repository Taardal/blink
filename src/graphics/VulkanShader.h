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
        VkDescriptorSetLayout descriptorSetLayout = nullptr;

    public:
        explicit VulkanShader(const VulkanShaderConfig& config);

        ~VulkanShader();

        operator VkShaderModule() const;

        VkDescriptorSetLayout getLayout() const;

        void setLayout(VkDescriptorSetLayout layout);

        void setLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
    };
}
