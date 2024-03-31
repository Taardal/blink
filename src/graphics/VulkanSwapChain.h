#pragma once

#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanApp.h"

namespace Blink {
    struct VulkanSwapChainConfig {
        Window* window = nullptr;
        VulkanApp* vulkanApp = nullptr;
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VulkanDevice* device = nullptr;
    };

    class VulkanSwapChain {
    private:
        VulkanSwapChainConfig config;
        VkSurfaceFormatKHR surfaceFormat{};
        VkPresentModeKHR presentMode{};
        VkExtent2D extent{};
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;

    public:
        explicit VulkanSwapChain(const VulkanSwapChainConfig& config);

        ~VulkanSwapChain();

        operator VkSwapchainKHR() const;

        const VkSurfaceFormatKHR& getSurfaceFormat() const;

        const VkExtent2D& getExtent() const;

        const std::vector<VkImageView>& getImageViews() const;

        bool initialize();

        void terminate() const;

        VkResult acquireNextImage(VkSemaphore semaphore, uint32_t* imageIndex) const;

    private:
        bool createSwapChain(uint32_t imageCount, const SwapChainInfo& swapChainInfo, const QueueFamilyIndices& queueFamilyIndices, VkSurfaceKHR surface);

        VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;

        VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;

        VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;

        uint32_t getImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;

        bool findImages(uint32_t imageCount);

        bool createImageViews();
    };
}
