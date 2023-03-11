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

        bool initialize();

        void terminate() const;

        VkResult createSwapChain(VkSwapchainCreateInfoKHR* createInfo, VkSwapchainKHR* swapchain) const;

        void destroySwapChain(VkSwapchainKHR swapChain) const;

        void getSwapChainImages(uint32_t* imageCount, std::vector<VkImage>* images, VkSwapchainKHR swapChain) const;

        VkResult createImageView(VkImageViewCreateInfo* createInfo, VkImageView* imageView) const;

        void destroyImageView(VkImageView imageView) const;
    private:

        bool createDevice(const QueueFamilyIndices& queueFamilyIndices);

        void destroyDevice() const;

        VkQueue getDeviceQueue(uint32_t queueFamilyIndex, uint32_t queueIndex = 0) const;
    };
}
