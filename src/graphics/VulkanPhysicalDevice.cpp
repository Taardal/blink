#include "pch.h"
#include "VulkanPhysicalDevice.h"

namespace Blink {

    VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanPhysicalDeviceConfig& config) noexcept(false) : config(config) {
        std::vector<VkPhysicalDevice> availableDevices = config.vulkanApp->getPhysicalDevices();
        BL_ASSERT_THROW(!availableDevices.empty());

        std::vector<const char*> requiredExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        if (Environment::isMacOS()) {
            requiredExtensions.push_back("VK_KHR_portability_subset");
        }
        const VulkanPhysicalDeviceInfo& mostSuitableDeviceInfo = getMostSuitableDevice(availableDevices, requiredExtensions);
        BL_ASSERT_THROW(mostSuitableDeviceInfo.physicalDevice != nullptr);

        this->deviceInfo = mostSuitableDeviceInfo;
        BL_LOG_INFO("Using physical device [{}]", mostSuitableDeviceInfo.properties.deviceName);
    }

    const VkPhysicalDeviceProperties& VulkanPhysicalDevice::getProperties() const {
        return deviceInfo.properties;
    }

    const VkPhysicalDeviceFeatures& VulkanPhysicalDevice::getFeatures() const {
        return deviceInfo.features;
    }

    const std::vector<VkExtensionProperties>& VulkanPhysicalDevice::getExtensions() const {
        return deviceInfo.extensions;
    }

    const QueueFamilyIndices& VulkanPhysicalDevice::getQueueFamilyIndices() const {
        return deviceInfo.queueFamilyIndices;
    }

    const SwapChainInfo& VulkanPhysicalDevice::getSwapChainInfo() const {
        return deviceInfo.swapChainInfo;
    }

    VkFormat VulkanPhysicalDevice::getDepthFormat() const {
        return deviceInfo.depthFormat;
    }

    void VulkanPhysicalDevice::updateSwapChainInfo() {
        deviceInfo.swapChainInfo = findSwapChainInfo(deviceInfo.physicalDevice);
    }

    VkResult VulkanPhysicalDevice::createDevice(VkDeviceCreateInfo* createInfo, VkDevice* device) const {
        return vkCreateDevice(deviceInfo.physicalDevice, createInfo, BL_VULKAN_ALLOCATOR, device);
    }

    uint32_t VulkanPhysicalDevice::getMemoryTypeIndex(
        const VkMemoryRequirements& memoryRequirements,
        VkMemoryPropertyFlags requiredMemoryProperties
    ) const {
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(deviceInfo.physicalDevice, &physicalDeviceMemoryProperties);
        for (uint32_t memoryTypeIndex = 0; memoryTypeIndex < physicalDeviceMemoryProperties.memoryTypeCount; memoryTypeIndex++) {
            bool memoryTypeIsSuitable = (memoryRequirements.memoryTypeBits & (1 << memoryTypeIndex)) > 0;
            if (!memoryTypeIsSuitable) {
                continue;
            }
            VkMemoryType& memoryType = physicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex];
            bool memoryTypeHasRequiredProperties = (memoryType.propertyFlags & requiredMemoryProperties) == requiredMemoryProperties;
            if (!memoryTypeHasRequiredProperties) {
                continue;
            }
            return memoryTypeIndex;
        }
        return -1;
    }

    VulkanPhysicalDeviceInfo VulkanPhysicalDevice::getMostSuitableDevice(
        const std::vector<VkPhysicalDevice>& physicalDevices,
        const std::vector<const char*>& requiredExtensions
    ) const {
        std::multimap<uint32_t , VulkanPhysicalDeviceInfo> devicesByRating;
        for (VkPhysicalDevice physicalDevice : physicalDevices) {
            VulkanPhysicalDeviceInfo deviceInfo = getDeviceInfo(physicalDevice, requiredExtensions);
            uint32_t suitabilityRating = getSuitabilityRating(deviceInfo, requiredExtensions);
            devicesByRating.insert(std::make_pair(suitabilityRating, deviceInfo));
        }
        uint32_t highestRating = devicesByRating.rbegin()->first;
        if (highestRating == 0) {
            return {};
        }
        return devicesByRating.rbegin()->second;
    }

    VulkanPhysicalDeviceInfo VulkanPhysicalDevice::getDeviceInfo(
        VkPhysicalDevice physicalDevice,
        const std::vector<const char*>& requiredExtensions
    ) const {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);

        VulkanPhysicalDeviceInfo deviceInfo{};
        deviceInfo.physicalDevice = physicalDevice;
        deviceInfo.properties = properties;
        deviceInfo.features = features;
        deviceInfo.extensions = findExtensions(physicalDevice, requiredExtensions);
        deviceInfo.queueFamilyIndices = findQueueFamilyIndices(physicalDevice);
        deviceInfo.swapChainInfo = findSwapChainInfo(physicalDevice);
        deviceInfo.depthFormat = findDepthFormat(physicalDevice);

        return deviceInfo;
    }

    std::vector<VkExtensionProperties> VulkanPhysicalDevice::findExtensions(
        VkPhysicalDevice physicalDevice,
        const std::vector<const char*>& requiredExtensions
    ) const {
        const char* layerName = nullptr;
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, layerName, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, layerName, &extensionCount, availableExtensions.data());

        std::vector<VkExtensionProperties> extensions;
        for (const char* requiredExtension : requiredExtensions) {
            for (const VkExtensionProperties& availableExtension : availableExtensions) {
                if (strcmp(requiredExtension, availableExtension.extensionName) == 0) {
                    extensions.push_back(availableExtension);
                }
            }
        }
        return extensions;
    }

    QueueFamilyIndices VulkanPhysicalDevice::findQueueFamilyIndices(VkPhysicalDevice physicalDevice) const {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        QueueFamilyIndices indices;
        for (int queueFamilyIndex = 0; queueFamilyIndex < queueFamilies.size(); queueFamilyIndex++) {
            const VkQueueFamilyProperties& queueFamily = queueFamilies[queueFamilyIndex];
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = queueFamilyIndex;
            }
            VkBool32 presentationSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, config.vulkanApp->getSurface(), &presentationSupport);
            if (presentationSupport) {
                indices.presentFamily = queueFamilyIndex;
            }
            if (hasRequiredQueueFamilyIndices(indices)) {
                break;
            }
        }
        return indices;
    }

    SwapChainInfo VulkanPhysicalDevice::findSwapChainInfo(VkPhysicalDevice physicalDevice) const {
        SwapChainInfo swapChainInfo;

        VkSurfaceKHR surface = config.vulkanApp->getSurface();

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapChainInfo.surfaceCapabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        swapChainInfo.surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapChainInfo.surfaceFormats.data());

        uint32_t presentationModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount, nullptr);
        swapChainInfo.presentModes.resize(presentationModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount, swapChainInfo.presentModes.data());

        return swapChainInfo;
    }

    VkFormat VulkanPhysicalDevice::findDepthFormat(VkPhysicalDevice physicalDevice) const {
        std::vector<VkFormat> depthFormats = {
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D24_UNORM_S8_UINT
        };
        VkFormatFeatureFlagBits requiredFeatures = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        for (VkFormat depthFormat : depthFormats) {
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, depthFormat, &formatProperties);
            if ((formatProperties.optimalTilingFeatures & requiredFeatures) == requiredFeatures) {
                return depthFormat;
            }
        }
        return VK_FORMAT_UNDEFINED;
    }

    uint32_t VulkanPhysicalDevice::getSuitabilityRating(
        const VulkanPhysicalDeviceInfo& physicalDeviceInfo,
        const std::vector<const char*>& requiredExtensions
    ) const {
        if (!hasRequiredFeatures(physicalDeviceInfo.features)) {
            BL_LOG_DEBUG("[{}] does not have required device features", physicalDeviceInfo.properties.deviceName);
            return 0;
        }
        if (!hasRequiredExtensions(requiredExtensions, physicalDeviceInfo.extensions)) {
            BL_LOG_DEBUG("[{}] does not have required device extensions", physicalDeviceInfo.properties.deviceName);
            return 0;
        }
        if (!hasRequiredQueueFamilyIndices(physicalDeviceInfo.queueFamilyIndices)) {
            BL_LOG_DEBUG("[{}] does not have required queue family indices", physicalDeviceInfo.properties.deviceName);
            return 0;
        }
        if (!hasRequiredSwapChainSupport(physicalDeviceInfo.swapChainInfo)) {
            BL_LOG_DEBUG("[{}] does not have required swap chain info", physicalDeviceInfo.properties.deviceName);
            return 0;
        }
        if (physicalDeviceInfo.depthFormat == VK_FORMAT_UNDEFINED) {
            BL_LOG_DEBUG("[{}] does not have a suitable depth format", physicalDeviceInfo.properties.deviceName);
            return 0;
        }
        uint32_t rating = physicalDeviceInfo.properties.limits.maxImageDimension2D;
        if (physicalDeviceInfo.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            rating += 1000;
        }
        return rating;
    }

    bool VulkanPhysicalDevice::hasRequiredExtensions(
        const std::vector<const char*>& extensions,
        const std::vector<VkExtensionProperties>& availableExtensions
    ) const {
        for (const char* extension: extensions) {
            bool extensionFound = false;
            for (const VkExtensionProperties& availableExtension: availableExtensions) {
                if (strcmp(extension, availableExtension.extensionName) == 0) {
                    extensionFound = true;
                    break;
                }
            }
            if (!extensionFound) {
                BL_LOG_ERROR("Could not find extension [{0}]", extension);
                return false;
            }
        }
        return true;
    }

    bool VulkanPhysicalDevice::hasRequiredQueueFamilyIndices(const QueueFamilyIndices& queueFamilyIndices) const {
        return queueFamilyIndices.graphicsFamily.has_value() && queueFamilyIndices.presentFamily.has_value();
    }

    bool VulkanPhysicalDevice::hasRequiredSwapChainSupport(const SwapChainInfo& swapChainInfo) const {
        return !swapChainInfo.surfaceFormats.empty() && !swapChainInfo.presentModes.empty();
    }

    bool VulkanPhysicalDevice::hasRequiredFeatures(const VkPhysicalDeviceFeatures& availableDeviceFeatures) const {
        return availableDeviceFeatures.samplerAnisotropy;
    }
}