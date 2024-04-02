#pragma once

#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Blink {
    struct VulkanRenderPassConfig {
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VulkanDevice* device = nullptr;
        VulkanSwapChain* swapChain = nullptr;
    };

    class VulkanRenderPass {
    private:
        VulkanRenderPassConfig config;
        VkRenderPass renderPass = VK_NULL_HANDLE;

    public:
        explicit VulkanRenderPass(const VulkanRenderPassConfig& config);

        ~VulkanRenderPass();

        operator VkRenderPass() const;

        void begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer) const;

        void end(VkCommandBuffer commandBuffer) const;
    };
}
