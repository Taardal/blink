#pragma once

#include "VulkanDevice.h"
#include <vulkan/vulkan.h>
#include <string>

namespace Blink {
    class VulkanShader {
    private:
        VulkanDevice* device;
        VkShaderModule shaderModule = VK_NULL_HANDLE;
        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

    public:
        explicit VulkanShader(VulkanDevice* device);

        operator VkShaderModule() const;

        bool initialize(const std::vector<char>& bytes);

        void terminate();

        VkDescriptorSetLayout getLayout() const;

        void setLayout(VkDescriptorSetLayout layout);

        void setLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
    };
}
