#pragma once

#include "VulkanApp.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanImage.h"

namespace Blink {
    struct VulkanSwapChainConfig {
        Window* window = nullptr;
        VulkanApp* vulkanApp = nullptr;
        VulkanDevice* device = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        uint32_t frameCount = 0;
    };

    class VulkanSwapChain {
    private:
        VulkanSwapChainConfig config;
        VkSurfaceFormatKHR surfaceFormat{};
        VkPresentModeKHR presentMode{};
        VkExtent2D extent{};
        uint32_t imageCount = 0;
        VkSwapchainKHR swapChain = nullptr;
        std::vector<VulkanImage*> colorImages;
        VulkanImage* depthImage = nullptr;
        VkRenderPass renderPass = nullptr;
        std::array<VkClearValue, 2> clearValues;
        std::vector<VkFramebuffer> framebuffers;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        uint32_t currentImageIndex = 0;
        VkFence currentInFlightFence = nullptr;
        VkSemaphore currentImageAvailableSemaphore = nullptr;
        VkSemaphore currentRenderFinishedSemaphore = nullptr;
        bool windowResized = false;

    public:
        explicit VulkanSwapChain(const VulkanSwapChainConfig& config);

        ~VulkanSwapChain();

        operator VkSwapchainKHR() const;

        VkRenderPass getRenderPass() const;

        const VkExtent2D& getExtent() const;

        void onEvent(Event& event);

        bool beginFrame(uint32_t frameIndex);

        void beginRenderPass(const VulkanCommandBuffer& commandBuffer) const;

        void endRenderPass(const VulkanCommandBuffer& commandBuffer) const;

        void endFrame(const VulkanCommandBuffer& commandBuffer);

    private:
        void recreateSwapChain();

        void createSwapChain();

        void destroySwapChain() const;

        void createColorImages();

        void destroyColorImages();

        void createDepthImage();

        void destroyDepthImage() const;

        void createRenderPass();

        void destroyRenderPass() const;

        void createFramebuffers();

        void destroyFramebuffers() const;

        void createSyncObjects();

        void destroySyncObjects() const;

        VkSurfaceFormatKHR getMostSuitableSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;

        VkPresentModeKHR getMostSuitablePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;

        VkExtent2D getMostSuitableExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;

        uint32_t getMostSuitableImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
    };
}
