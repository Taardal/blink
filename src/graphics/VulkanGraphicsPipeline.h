#pragma once

#include "graphics/VulkanDevice.h"
#include "graphics/VulkanShader.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct VulkanGraphicsPipelineConfig {
        VulkanDevice* device = nullptr;
        VkRenderPass renderPass = nullptr;
        std::shared_ptr<VulkanShader> vertexShader;
        std::shared_ptr<VulkanShader> fragmentShader;
        VkVertexInputBindingDescription* vertexBindingDescription;
        std::vector<VkVertexInputAttributeDescription>* vertexAttributeDescriptions;
        std::vector<VkDescriptorSetLayout>* descriptorSetLayouts;
        std::vector<VkPushConstantRange>* pushConstantRanges;
        bool depthTestEnabled = false;
        VkPipelineDepthStencilStateCreateInfo* depthStencilCreateInfo;
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
