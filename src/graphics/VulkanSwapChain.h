#pragma once

#include "VulkanApp.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanImage.h"

namespace Blink {
    struct VulkanSwapChainConfig {
        Window* window = nullptr;
        VulkanApp* vulkanApp = nullptr;
        VulkanPhysicalDevice* physicalDevice = nullptr;
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
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> colorImages;
        std::vector<VkImageView> colorImageViews;
        VulkanImage* depthImage = nullptr;
        VkImageView depthImageView;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        std::array<VkClearValue, 2> clearValues;
        std::vector<VkFramebuffer> framebuffers;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        uint32_t imageIndex = 0;
        VkFence inFlightFence = VK_NULL_HANDLE;
        VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
        VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
        bool framebufferResized = false;

    public:
        explicit VulkanSwapChain(const VulkanSwapChainConfig& config) noexcept(false);

        ~VulkanSwapChain();

        operator VkSwapchainKHR() const;

        VkRenderPass getRenderPass() const;

        const VkExtent2D& getExtent() const;

        void onEvent(Event& event);

        bool beginFrame(uint32_t frameIndex) noexcept(false);

        void beginRenderPass(const VulkanCommandBuffer& commandBuffer) noexcept(false);

        void endRenderPass(const VulkanCommandBuffer& commandBuffer) noexcept(false);

        void endFrame(const VulkanCommandBuffer& commandBuffer) noexcept(false);

    private:
        void recreate() noexcept(false);

        void createSwapChain() noexcept(false);

        void destroySwapChain() const;

        void createColorImages() noexcept(false);

        void destroyColorImages();

        void createDepthImage() noexcept(false);

        void destroyDepthImage() const;

        void createRenderPass() noexcept(false);

        void destroyRenderPass();

        void createFramebuffers() noexcept(false);

        void destroyFramebuffers();

        void createSyncObjects() noexcept(false);

        void destroySyncObjects();

        VkSurfaceFormatKHR getMostSuitableSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;

        VkPresentModeKHR getMostSuitablePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;

        VkExtent2D getMostSuitableExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;

        uint32_t getMostSuitableImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;
    };
}
