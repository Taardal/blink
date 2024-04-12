#pragma once

#include "AppConfig.h"
#include "VulkanPhysicalDevice.h"
#include <vulkan/vulkan.h>

namespace Blink {

    struct QueueInfo {
        uint32_t queueIndex = -1;
        uint32_t queueFamilyIndex = -1;
    };

    struct VulkanDeviceConfig {
        VulkanPhysicalDevice* physicalDevice = nullptr;
    };

    class VulkanDevice {
    private:
        VulkanDeviceConfig config;
        VkDevice device = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;

    public:
        explicit VulkanDevice(const VulkanDeviceConfig& config) noexcept(false);

        ~VulkanDevice();

        operator VkDevice() const;

        VulkanPhysicalDevice* getPhysicalDevice() const;

        VkQueue getGraphicsQueue() const;

        VkResult submitToGraphicsQueue(VkSubmitInfo* submitInfo, VkFence fence = nullptr) const;

        VkResult waitUntilGraphicsQueueIsIdle() const;

        VkQueue getPresentQueue() const;

        VkResult submitToPresentQueue(VkPresentInfoKHR* presentInfo) const;

        VkResult waitUntilPresentQueueIsIdle() const;

        VkResult waitUntilIdle() const;

        VkResult waitUntilQueueIsIdle(VkQueue queue) const;

        VkResult createSwapChain(VkSwapchainCreateInfoKHR* createInfo, VkSwapchainKHR* swapchain) const;

        void destroySwapChain(VkSwapchainKHR swapChain) const;

        VkResult getSwapChainImages(VkSwapchainKHR swapChain, uint32_t* imageCount, std::vector<VkImage>* images) const;

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

        VkResult waitForFence(VkFence* fence) const;

        VkResult resetFences(uint32_t count, VkFence* fences) const;

        VkResult resetFence(VkFence* fence) const;

        VkResult acquireSwapChainImage(VkSwapchainKHR swapChain, VkSemaphore semaphore, uint32_t* imageIndex) const;

        VkResult createBuffer(VkBufferCreateInfo* createInfo, VkBuffer* buffer) const;

        void destroyBuffer(VkBuffer buffer) const;

        VkMemoryRequirements getBufferMemoryRequirements(VkBuffer buffer) const;

        VkResult allocateMemory(VkMemoryAllocateInfo* allocateInfo, VkDeviceMemory* memory) const;

        void freeMemory(VkDeviceMemory memory) const;

        VkResult bindBufferMemory(VkBuffer buffer, VkDeviceMemory memory) const;

        VkResult mapMemory(VkDeviceMemory memory, VkDeviceSize memorySize, void** data) const;

        void unmapMemory(VkDeviceMemory memory) const;

        VkResult createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* createInfo, VkDescriptorSetLayout* layout) const;

        void destroyDescriptorSetLayout(VkDescriptorSetLayout layout) const;

        VkResult createDescriptorPool(VkDescriptorPoolCreateInfo* createInfo, VkDescriptorPool* pool) const;

        void destroyDescriptorPool(VkDescriptorPool pool) const;

        VkResult allocateDescriptorSets(VkDescriptorSetAllocateInfo* allocateInfo, VkDescriptorSet* descriptorSets) const;

        void updateDescriptorSets(uint32_t count, VkWriteDescriptorSet* write) const;

        VkResult createImage(VkImageCreateInfo* createInfo, VkImage* image) const;

        VkMemoryRequirements getImageMemoryRequirements(VkImage image) const;

        VkResult bindImageMemory(VkImage image, VkDeviceMemory memory) const;

        void destroyImage(VkImage image) const;

        VkResult createSampler(VkSamplerCreateInfo* createInfo, VkSampler* sampler) const;

        void destroySampler(VkSampler sampler) const;

    private:

        void createDevice(const QueueFamilyIndices& queueFamilyIndices) noexcept(false);

        void destroyDevice() const;

        VkQueue getDeviceQueue(uint32_t queueFamilyIndex) const;
    };
}
