#pragma once

#include "VulkanApp.h"
#include <vulkan/vulkan.h>

namespace Blink {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
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
        VkFormat depthFormat = VK_FORMAT_UNDEFINED;
    };

    struct VulkanPhysicalDeviceConfig {
        VulkanApp* vulkanApp = nullptr;
    };

    class VulkanPhysicalDevice {
    public:
        static const std::vector<const char*> requiredExtensions;

    private:
        VulkanPhysicalDeviceConfig config;
        VulkanPhysicalDeviceInfo deviceInfo{};

    public:
        explicit VulkanPhysicalDevice(const VulkanPhysicalDeviceConfig& config);

        const std::vector<VkExtensionProperties>& getExtensions() const;

        const VkPhysicalDeviceFeatures& getFeatures() const;

        const VkPhysicalDeviceProperties& getProperties() const;

        VkFormat getDepthFormat() const;

        const QueueFamilyIndices& getQueueFamilyIndices() const;

        const SwapChainInfo& getSwapChainInfo() const;

        void updateSwapChainInfo();

        VkResult createDevice(VkDeviceCreateInfo* createInfo, VkDevice* device) const;

        uint32_t getMemoryTypeIndex(
            const VkMemoryRequirements& memoryRequirements,
            VkMemoryPropertyFlags requiredMemoryProperties
        ) const;

    private:
        VulkanPhysicalDeviceInfo getMostSuitableDevice(
            const std::vector<VkPhysicalDevice>& physicalDevices,
            const std::vector<const char*>& requiredExtensions
        ) const;

        VulkanPhysicalDeviceInfo getDeviceInfo(
            VkPhysicalDevice physicalDevice,
            const std::vector<const char*>& requiredExtensions
        ) const;

        std::vector<VkExtensionProperties> findExtensions(
            VkPhysicalDevice physicalDevice,
            const std::vector<const char*>& requiredExtensions
        ) const;

        QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice physicalDevice) const;

        SwapChainInfo findSwapChainInfo(VkPhysicalDevice physicalDevice) const;

        VkFormat findDepthFormat(VkPhysicalDevice physicalDevice) const;

        uint32_t getSuitabilityRating(
            const VulkanPhysicalDeviceInfo& physicalDeviceInfo,
            const std::vector<const char*>& requiredExtensions
        ) const;

        bool hasRequiredExtensions(
            const std::vector<const char*>& extensions,
            const std::vector<VkExtensionProperties>& availableExtensions
        ) const;

        bool hasRequiredQueueFamilyIndices(const QueueFamilyIndices& queueFamilyIndices) const;

        bool hasRequiredSwapChainSupport(const SwapChainInfo& swapChainInfo) const;

        bool hasRequiredFeatures(const VkPhysicalDeviceFeatures& availableDeviceFeatures) const;
    };
}
