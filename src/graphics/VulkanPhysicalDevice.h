#pragma once

#include "Vulkan.h"
#include <vulkan/vulkan.h>

namespace Blink {

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentationFamily;
    };

    struct SwapChainInfo {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct VulkanPhysicalDeviceInfo {
        VkPhysicalDevice physicalDevice = nullptr;
        VkPhysicalDeviceProperties properties{};
        VkPhysicalDeviceFeatures features{};
        std::vector<VkExtensionProperties> extensions{};
        QueueFamilyIndices queueFamilyIndices{};
        SwapChainInfo swapChainInfo{};
    };
}

namespace Blink {
    class VulkanPhysicalDevice {
    public:
        static const std::vector<const char*> requiredExtensions;

    private:
        Vulkan* vulkan;
        VulkanPhysicalDeviceInfo deviceInfo{};

    public:
        explicit VulkanPhysicalDevice(Vulkan* vulkan);

        const QueueFamilyIndices& getQueueFamilyIndices() const;

        const VkPhysicalDeviceFeatures& getFeatures() const;

        bool initialize();

        void terminate();

        VkResult createDevice(const VkDeviceCreateInfo& vkDeviceCreateInfo, VkDevice* vkDevice) const;

    private:
        VulkanPhysicalDeviceInfo getMostSuitableDevice(const std::vector<VkPhysicalDevice>& vkPhysicalDevices) const;

        VulkanPhysicalDeviceInfo getDeviceInfo(VkPhysicalDevice vkPhysicalDevice) const;

        uint32_t getSuitabilityRating(const VulkanPhysicalDeviceInfo& deviceInfo) const;

        QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device) const;

        std::vector<VkExtensionProperties> findRequiredExtensions() const;

        bool hasRequiredQueueFamilyIndices(const QueueFamilyIndices& queueFamilyIndices) const;
    };
}
