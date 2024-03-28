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

        VkDescriptorSetLayout getDescriptorLayout() const;

        void setDescriptorLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

        void createDescriptorSets(uint16_t descriptorSetCount, VkDescriptorPool descriptorPool);

        typedef uint16_t BindingIndex;

        void setBufferDescriptors(
            const std::vector<std::tuple<BindingIndex, VkDescriptorType, VkDescriptorBufferInfo>>& bufferDescriptors
        ) const;
    };
}
