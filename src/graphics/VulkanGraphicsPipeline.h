#pragma once

#include "graphics/VulkanShader.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct VulkanGraphicsPipelineConfig {
        VulkanDevice* device = nullptr;
        VulkanSwapChain* swapChain = nullptr;
        VulkanShader* vertexShader = nullptr;
        VulkanShader* fragmentShader = nullptr;
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    };

    class VulkanGraphicsPipeline {
    private:
        VulkanGraphicsPipelineConfig config;
        VkPipelineLayout layout = nullptr;
        VkPipeline pipeline = nullptr;

    public:
        explicit VulkanGraphicsPipeline(const VulkanGraphicsPipelineConfig& config);

        ~VulkanGraphicsPipeline();

        VkPipelineLayout getLayout() const;

        void bind(VkCommandBuffer commandBuffer) const;
    };
}
