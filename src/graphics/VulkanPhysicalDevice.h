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
    };

    class VulkanPhysicalDevice {
    public:
        static const std::vector<const char*> requiredExtensions;

    private:
        VulkanApp* vulkan;
        VulkanPhysicalDeviceInfo deviceInfo{};

    public:
        explicit VulkanPhysicalDevice(VulkanApp* vulkan);

        const std::vector<VkExtensionProperties>& getExtensions() const;

        const QueueFamilyIndices& getQueueFamilyIndices() const;

        const VkPhysicalDeviceFeatures& getFeatures() const;

        const VkPhysicalDeviceProperties& getProperties() const;

        const SwapChainInfo& getSwapChainInfo() const;

        void updateSwapChainInfo();

        VkResult createDevice(VkDeviceCreateInfo* createInfo, VkDevice* device) const;

        uint32_t getMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryPropertyFlags) const;

        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

    private:
        VulkanPhysicalDeviceInfo getMostSuitableDevice(const std::vector<VkPhysicalDevice>& vkPhysicalDevices, const std::vector<const char*>& requiredExtensions) const;

        VulkanPhysicalDeviceInfo getDeviceInfo(VkPhysicalDevice vkPhysicalDevice, const std::vector<const char*>& requiredExtensions) const;

        std::vector<VkExtensionProperties> findExtensions(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions) const;

        QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device) const;

        SwapChainInfo findSwapChainInfo(VkPhysicalDevice device) const;

        uint32_t getSuitabilityRating(const VulkanPhysicalDeviceInfo& deviceInfo, const std::vector<const char*>& requiredExtensions) const;

        bool hasRequiredExtensions(const std::vector<const char*>& extensions, const std::vector<VkExtensionProperties>& availableExtensions) const;

        bool hasRequiredQueueFamilyIndices(const QueueFamilyIndices& queueFamilyIndices) const;

        bool hasRequiredSwapChainSupport(const SwapChainInfo& swapChainInfo) const;

        bool hasRequiredFeatures(const VkPhysicalDeviceFeatures& availableDeviceFeatures) const;
    };
}
