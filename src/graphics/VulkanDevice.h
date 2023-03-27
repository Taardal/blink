#pragma once

#include "AppConfig.h"
#include "VulkanPhysicalDevice.h"
#include <vulkan/vulkan.h>

namespace Blink {

    struct QueueInfo {
        uint32_t queueIndex = -1;
        uint32_t queueFamilyIndex = -1;
    };

    class VulkanDevice {
    private:
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VkDevice device = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;

    public:
        explicit VulkanDevice(VulkanPhysicalDevice* physicalDevice);

        VkQueue getGraphicsQueue() const;

        VkQueue getPresentQueue() const;

        bool initialize();

        void terminate() const;

        void waitUntilIdle() const;

        VkResult createSwapChain(VkSwapchainCreateInfoKHR* createInfo, VkSwapchainKHR* swapchain) const;

        void destroySwapChain(VkSwapchainKHR swapChain) const;

        void getSwapChainImages(uint32_t* imageCount, std::vector<VkImage>* images, VkSwapchainKHR swapChain) const;

        VkResult createImageView(VkImageViewCreateInfo* createInfo, VkImageView* imageView) const;

        void destroyImageView(VkImageView imageView) const;

        VkResult createShaderModule(VkShaderModuleCreateInfo* createInfo, VkShaderModule* shaderModule) const;

        void destroyShaderModule(VkShaderModule shaderModule) const;

        VkResult createPipelineLayout(VkPipelineLayoutCreateInfo* createInfo, VkPipelineLayout* layout) const;

        void destroyPipelineLayout(VkPipelineLayout layout) const;

        VkResult createRenderPass(VkRenderPassCreateInfo* createInfo, VkRenderPass* renderPass) const;

        void destroyRenderPass(VkRenderPass renderPass) const;

        VkResult createGraphicsPipeline(VkGraphicsPipelineCreateInfo* createInfo, VkPipeline* pipeline) const;

        void destroyGraphicsPipeline(VkPipeline pipeline) const;

        VkResult createFramebuffer(VkFramebufferCreateInfo* createInfo, VkFramebuffer* framebuffer) const;

        void destroyFramebuffer(VkFramebuffer framebuffer) const;

        VkResult createCommandPool(VkCommandPoolCreateInfo* createInfo, VkCommandPool* commandPool) const;

        void destroyCommandPool(VkCommandPool commandPool) const;

        VkResult allocateCommandBuffers(VkCommandBufferAllocateInfo* allocateInfo, VkCommandBuffer* commandBuffers) const;

        void freeCommandBuffers(uint32_t count, VkCommandBuffer* commandBuffers, VkCommandPool commandPool) const;

        VkResult createSemaphore(VkSemaphoreCreateInfo* createInfo, VkSemaphore* semaphore) const;

        void destroySemaphore(VkSemaphore semaphore) const;

        VkResult createFence(VkFenceCreateInfo* createInfo, VkFence* fence) const;

        void destroyFence(VkFence fence) const;

        void waitForFence(VkFence* fence) const;

        void resetFences(uint32_t count, VkFence* fences) const;

        void resetFence(VkFence* fence) const;

        VkResult acquireSwapChainImage(VkSwapchainKHR swapChain, VkSemaphore semaphore, uint32_t* imageIndex) const;

    private:

        bool createDevice(const QueueFamilyIndices& queueFamilyIndices);

        void destroyDevice() const;

        VkQueue getDeviceQueue(uint32_t queueFamilyIndex, uint32_t queueIndex = 0) const;
    };
}
