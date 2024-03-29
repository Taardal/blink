#pragma once

#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Blink {
    class VulkanRenderPass {
    private:
        VulkanSwapChain* swapChain;
        VulkanDevice* device;
        VulkanPhysicalDevice* physicalDevice;
        VkRenderPass renderPass = nullptr;

    public:
        VulkanRenderPass(VulkanSwapChain* swapChain, VulkanDevice* device, VulkanPhysicalDevice* physicalDevice);

        ~VulkanRenderPass();

        operator VkRenderPass() const;

        void begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer) const;

        void end(VkCommandBuffer commandBuffer) const;

        VkFormat findDepthFormat();
    };
}
