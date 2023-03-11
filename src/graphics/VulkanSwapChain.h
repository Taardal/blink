#pragma once

#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include "Vulkan.h"

namespace Blink {
    class VulkanSwapChain {
    private:
        VulkanDevice* vulkanDevice;
        VulkanPhysicalDevice* vulkanPhysicalDevice;
        Vulkan* vulkan;
        Window* window;
        VkSurfaceFormatKHR surfaceFormat{};
        VkPresentModeKHR presentMode{};
        VkExtent2D extent{};
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;

    public:
        VulkanSwapChain(VulkanDevice* vulkanDevice, VulkanPhysicalDevice* vulkanPhysicalDevice, Vulkan* vulkan, Window* window);

        bool initialize();

        void terminate();

    private:
        bool createSwapChain(uint32_t imageCount, const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

        VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;

        VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;

        VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;

        uint32_t getImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;

        bool findImages(uint32_t imageCount);

        bool createImageViews();
    };
}
