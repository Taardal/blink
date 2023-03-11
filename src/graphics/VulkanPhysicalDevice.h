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

    struct VulkanPhysicalDeviceConfig {
        std::vector<const char*> requiredExtensions;
    };

    class VulkanPhysicalDevice {
    public:
        static const std::vector<const char*> requiredExtensions;

    private:
        Vulkan* vulkan = nullptr;
        VulkanPhysicalDeviceInfo deviceInfo{};

    public:
        explicit VulkanPhysicalDevice(Vulkan* vulkan);

        const std::vector<VkExtensionProperties>& getExtensions() const;

        const QueueFamilyIndices& getQueueFamilyIndices() const;

        const VkPhysicalDeviceFeatures& getFeatures() const;

        VkResult createDevice(const VkDeviceCreateInfo& vkDeviceCreateInfo, VkDevice* vkDevice) const;

        bool initialize(const VulkanPhysicalDeviceConfig& config);

        void terminate() const;

    private:
        VulkanPhysicalDeviceInfo getMostSuitableDevice(const std::vector<VkPhysicalDevice>& vkPhysicalDevices, const VulkanPhysicalDeviceConfig& config) const;

        VulkanPhysicalDeviceInfo getDeviceInfo(VkPhysicalDevice vkPhysicalDevice, const VulkanPhysicalDeviceConfig& config) const;

        std::vector<VkExtensionProperties> findExtensions(VkPhysicalDevice device, const VulkanPhysicalDeviceConfig& config) const;

        QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device) const;

        //SwapChainInfo findSwapChainInfo(VkPhysicalDevice device) const;

        uint32_t getSuitabilityRating(const VulkanPhysicalDeviceInfo& deviceInfo, const VulkanPhysicalDeviceConfig& config) const;

        bool hasRequiredExtensions(const std::vector<const char*>& extensions, const std::vector<VkExtensionProperties>& availableExtensions) const;

        bool hasRequiredQueueFamilyIndices(const QueueFamilyIndices& queueFamilyIndices) const;

        bool hasRequiredSwapChainSupport(const SwapChainInfo& swapChainInfo) const;

        bool hasRequiredFeatures(const VkPhysicalDeviceFeatures& availableDeviceFeatures) const;
    };
}
