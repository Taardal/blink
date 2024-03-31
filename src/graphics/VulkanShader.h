#pragma once

#include "VulkanDevice.h"
#include <vulkan/vulkan.h>
#include <string>

namespace Blink {
    struct VulkanShaderConfig {
        VulkanDevice* device = nullptr;
    };

    class VulkanShader {
    private:
        VulkanShaderConfig config;
        VkShaderModule shaderModule = VK_NULL_HANDLE;
        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

    public:
        explicit VulkanShader(const VulkanShaderConfig& config);

        operator VkShaderModule() const;

        bool initialize(const std::vector<char>& bytes);

        void terminate();

        VkDescriptorSetLayout getLayout() const;

        void setLayout(VkDescriptorSetLayout layout);

        void setLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
    };
}
