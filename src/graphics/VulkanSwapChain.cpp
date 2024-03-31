#include "VulkanSwapChain.h"

namespace Blink {

    VulkanSwapChain::VulkanSwapChain(const VulkanSwapChainConfig& config) : config(config) {
        initialize();
    }

    VulkanSwapChain::~VulkanSwapChain() {
        terminate();
    }

    VulkanSwapChain::operator VkSwapchainKHR() const {
        return swapChain;
    }

    const VkSurfaceFormatKHR& VulkanSwapChain::getSurfaceFormat() const {
        return surfaceFormat;
    }

    const VkExtent2D& VulkanSwapChain::getExtent() const {
        return extent;
    }

    const std::vector<VkImageView>& VulkanSwapChain::getImageViews() const {
        return imageViews;
    }

    bool VulkanSwapChain::initialize() {
        const SwapChainInfo& swapChainInfo = config.physicalDevice->getSwapChainInfo();
        const QueueFamilyIndices& queueFamilyIndices = config.physicalDevice->getQueueFamilyIndices();
        VkSurfaceKHR surface = config.vulkanApp->getSurface();

        surfaceFormat = chooseSurfaceFormat(swapChainInfo.surfaceFormats);
        presentMode = choosePresentMode(swapChainInfo.presentModes);
        extent = chooseExtent(swapChainInfo.surfaceCapabilities);

        uint32_t imageCount = getImageCount(swapChainInfo.surfaceCapabilities);

        if (!createSwapChain(imageCount, swapChainInfo, queueFamilyIndices, surface)) {
            BL_LOG_ERROR("Could not create swap chain");
            return false;
        }

        if (!findImages(imageCount)) {
            BL_LOG_ERROR("Could not get swap chain images");
            return false;
        }
        if (!createImageViews()) {
            BL_LOG_ERROR("Could not create swap chain image views");
            return false;
        }

        return true;
    }

    void VulkanSwapChain::terminate() const {
        for (VkImageView imageView : imageViews) {
            config.device->destroyImageView(imageView);
        }
        config.device->destroySwapChain(swapChain);
        BL_LOG_INFO("Destroyed swap chain");
    }

    VkResult VulkanSwapChain::acquireNextImage(VkSemaphore semaphore, uint32_t* imageIndex) const {
        return config.device->acquireSwapChainImage(swapChain, semaphore, imageIndex);
    }

    bool VulkanSwapChain::createSwapChain(uint32_t imageCount, const SwapChainInfo& swapChainInfo, const QueueFamilyIndices& queueFamilyIndices, VkSurfaceKHR surface) {
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.minImageCount = imageCount;
        createInfo.surface = surface;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform = swapChainInfo.surfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentFamily) {
            std::vector<uint32_t> queueFamilyIndexValues = {
                    queueFamilyIndices.graphicsFamily.value(),
                    queueFamilyIndices.presentFamily.value()
            };
            createInfo.pQueueFamilyIndices = queueFamilyIndexValues.data();
            createInfo.queueFamilyIndexCount = queueFamilyIndexValues.size();
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        } else {
            createInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        return config.device->createSwapChain(&createInfo, &swapChain) == VK_SUCCESS;
    }

    VkSurfaceFormatKHR VulkanSwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapChain::choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapChain::chooseExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const {
        bool extentMustMatchWindowSize = surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max();
        if (extentMustMatchWindowSize) {
            return surfaceCapabilities.currentExtent;
        }
        WindowSize windowSizeInPixels = config.window->getSizeInPixels();
        uint32_t width = std::clamp((uint32_t) windowSizeInPixels.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        uint32_t height = std::clamp((uint32_t) windowSizeInPixels.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        return { width, height };
    }

    uint32_t VulkanSwapChain::getImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const {
        uint32_t minImageCount = surfaceCapabilities.minImageCount;
        uint32_t maxImageCount = surfaceCapabilities.maxImageCount;
        uint32_t imageCount = minImageCount + 1;
        if (maxImageCount > 0 && imageCount > maxImageCount) {
            imageCount = maxImageCount;
        }
        return imageCount;
    }

    bool VulkanSwapChain::findImages(uint32_t imageCount) {
        config.device->getSwapChainImages(&imageCount, &images, swapChain);
        return !images.empty();
    }

    bool VulkanSwapChain::createImageViews() {
        imageViews.resize(images.size());
        for (size_t i = 0; i < images.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = surfaceFormat.format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            if (config.device->createImageView(&createInfo, &imageViews[i]) != VK_SUCCESS) {
                return false;
            }
        }
        return true;
    }
}