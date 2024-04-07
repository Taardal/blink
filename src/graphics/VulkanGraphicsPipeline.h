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
        VkDescriptorSetLayout descriptorSetLayout = nullptr;
    };

    class VulkanGraphicsPipeline {
    private:
        VulkanGraphicsPipelineConfig config;
        VkPipelineLayout layout = nullptr;
        VkPipeline pipeline = nullptr;

    public:
        explicit VulkanGraphicsPipeline(const VulkanGraphicsPipelineConfig& config) noexcept(false);

        ~VulkanGraphicsPipeline();

        VkPipelineLayout getLayout() const;

        void bind(VkCommandBuffer commandBuffer) const;
    };
}
