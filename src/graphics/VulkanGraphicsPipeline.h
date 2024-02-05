#pragma once

#include "graphics/VulkanShader.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Blink {

    struct VulkanGraphicsPipelineConfig {
    };

    class VulkanGraphicsPipeline {
    private:
        VulkanShader* vertexShader;
        VulkanShader* fragmentShader;
        VulkanRenderPass* renderPass;
        VulkanSwapChain* swapChain;
        VulkanDevice* device;
        VkPipelineLayout layout = nullptr;
        VkPipeline pipeline = nullptr;

    public:
        VulkanGraphicsPipeline(VulkanShader* vertexShader, VulkanShader* fragmentShader, VulkanRenderPass* renderPass, VulkanSwapChain* swapChain, VulkanDevice* device);

        VkPipelineLayout getLayout() const;

        bool initialize(VkDescriptorSetLayout descriptorSetLayout);

        void terminate();

        void bind(VkCommandBuffer commandBuffer) const;
    };
}
