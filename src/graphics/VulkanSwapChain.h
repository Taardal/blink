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
        explicit VulkanSwapChain(const VulkanSwapChainConfig& config) noexcept(false);

        ~VulkanSwapChain();

        operator VkSwapchainKHR() const;

        VkRenderPass getRenderPass() const;

        const VkExtent2D& getExtent() const;

        void onEvent(Event& event);

        bool beginFrame(uint32_t frameIndex) noexcept(false);

        void beginRenderPass(const VulkanCommandBuffer& commandBuffer) const;

        void endRenderPass(const VulkanCommandBuffer& commandBuffer) const;

        void endFrame(const VulkanCommandBuffer& commandBuffer) noexcept(false);

    private:
        void recreateSwapChain() noexcept(false);

        void createSwapChain() noexcept(false);

        void destroySwapChain() const;

        void createColorImages() noexcept(false);

        void destroyColorImages();

        void createDepthImage() noexcept(false);

        void destroyDepthImage() const;

        void createRenderPass() noexcept(false);

        void destroyRenderPass() const;

        void createFramebuffers() noexcept(false);

        void destroyFramebuffers() const;

        void createSyncObjects() noexcept(false);

        void destroySyncObjects() const;

        VkSurfaceFormatKHR getMostSuitableSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;

        VkPresentModeKHR getMostSuitablePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;

        VkExtent2D getMostSuitableExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;

        uint32_t getMostSuitableImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
    };
}
