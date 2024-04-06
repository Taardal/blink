#pragma once

#include "graphics/VulkanShader.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct VulkanGraphicsPipelineConfig {
        VulkanShader* vertexShader = nullptr;
        VulkanShader* fragmentShader = nullptr;
        VulkanSwapChain* swapChain = nullptr;
        VulkanDevice* device = nullptr;
    };

    class VulkanGraphicsPipeline {
    private:
        VulkanGraphicsPipelineConfig config;
        VkPipelineLayout layout = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;

    public:
        explicit VulkanGraphicsPipeline(const VulkanGraphicsPipelineConfig& config);

        VkPipelineLayout getLayout() const;

        bool initialize(VkDescriptorSetLayout descriptorSetLayout);

        void terminate();

        void bind(VkCommandBuffer commandBuffer) const;
    };
}
