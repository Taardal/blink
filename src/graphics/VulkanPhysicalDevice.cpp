#include "pch.h"
#include "VulkanPhysicalDevice.h"

namespace Blink {

    VulkanPhysicalDevice::VulkanPhysicalDevice(Vulkan* vulkan)
            : vulkan(vulkan) {}

    const std::vector<VkExtensionProperties>& VulkanPhysicalDevice::getExtensions() const {
        return deviceInfo.extensions;
    }

    const QueueFamilyIndices& VulkanPhysicalDevice::getQueueFamilyIndices() const {
        return deviceInfo.queueFamilyIndices;
    }

    const VkPhysicalDeviceFeatures& VulkanPhysicalDevice::getFeatures() const {
        return deviceInfo.features;
    }

    const SwapChainInfo& VulkanPhysicalDevice::getSwapChainInfo() const {
        return deviceInfo.swapChainInfo;
    }

    VkResult VulkanPhysicalDevice::createDevice(VkDeviceCreateInfo* vkDeviceCreateInfo, VkDevice* vkDevice) const {
        return vkCreateDevice(deviceInfo.physicalDevice, vkDeviceCreateInfo, BL_VK_ALLOCATOR, vkDevice);
    }

    void VulkanPhysicalDevice::updateSwapChainInfo() {
        deviceInfo.swapChainInfo = findSwapChainInfo(deviceInfo.physicalDevice);
    }

    bool VulkanPhysicalDevice::initialize() {
        std::vector<VkPhysicalDevice> availableDevices = vulkan->getPhysicalDevices();
        if (availableDevices.empty()) {
            BL_LOG_ERROR("Could not find any physical devices");
            return false;
        }
        std::vector<const char*> requiredExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        if (Environment::isMacOS()) {
            requiredExtensions.push_back("VK_KHR_portability_subset");
        }
        VulkanPhysicalDeviceInfo mostSuitableDeviceInfo = getMostSuitableDevice(availableDevices, requiredExtensions);
        if (mostSuitableDeviceInfo.physicalDevice == nullptr) {
            BL_LOG_ERROR("Could not get any suitable device");
            return false;
        }
        this->deviceInfo = mostSuitableDeviceInfo;
        BL_LOG_INFO("Using physical device [{}]", mostSuitableDeviceInfo.properties.deviceName);
        return true;
    }

    void VulkanPhysicalDevice::terminate() const {

    }

    VulkanPhysicalDeviceInfo VulkanPhysicalDevice::getMostSuitableDevice(const std::vector<VkPhysicalDevice>& vkPhysicalDevices, const std::vector<const char*>& requiredExtensions) const {
        std::multimap<uint32_t , VulkanPhysicalDeviceInfo> devicesByRating;
        for (VkPhysicalDevice vkPhysicalDevice : vkPhysicalDevices) {
            VulkanPhysicalDeviceInfo deviceInfo = getDeviceInfo(vkPhysicalDevice, requiredExtensions);
            uint32_t suitabilityRating = getSuitabilityRating(deviceInfo, requiredExtensions);
            devicesByRating.insert(std::make_pair(suitabilityRating, deviceInfo));
        }
        uint32_t highestRating = devicesByRating.rbegin()->first;
        if (highestRating == 0) {
            return {};
        }
        return devicesByRating.rbegin()->second;
    }

    VulkanPhysicalDeviceInfo VulkanPhysicalDevice::getDeviceInfo(VkPhysicalDevice vkPhysicalDevice, const std::vector<const char*>& requiredExtensions) const {
        VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &vkPhysicalDeviceProperties);

        VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures;
        vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &vkPhysicalDeviceFeatures);

        VulkanPhysicalDeviceInfo deviceInfo{};
        deviceInfo.physicalDevice = vkPhysicalDevice;
        deviceInfo.properties = vkPhysicalDeviceProperties;
        deviceInfo.features = vkPhysicalDeviceFeatures;
        deviceInfo.extensions = findExtensions(vkPhysicalDevice, requiredExtensions);
        deviceInfo.queueFamilyIndices = findQueueFamilyIndices(vkPhysicalDevice);
        deviceInfo.swapChainInfo = findSwapChainInfo(vkPhysicalDevice);

        return deviceInfo;
    }

    std::vector<VkExtensionProperties> VulkanPhysicalDevice::findExtensions(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions) const {
        const char* layerName = nullptr;
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(device, layerName, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, layerName, &extensionCount, availableExtensions.data());

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

    QueueFamilyIndices VulkanPhysicalDevice::findQueueFamilyIndices(VkPhysicalDevice vkPhysicalDevice) const {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

        QueueFamilyIndices indices;
        for (int queueFamilyIndex = 0; queueFamilyIndex < queueFamilies.size(); queueFamilyIndex++) {
            const VkQueueFamilyProperties& queueFamily = queueFamilies[queueFamilyIndex];
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = queueFamilyIndex;
            }
            VkBool32 presentationSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, queueFamilyIndex, vulkan->getSurface(), &presentationSupport);
            if (presentationSupport) {
                indices.presentFamily = queueFamilyIndex;
            }
            if (hasRequiredQueueFamilyIndices(indices)) {
                break;
            }
        }
        return indices;
    }

    SwapChainInfo VulkanPhysicalDevice::findSwapChainInfo(VkPhysicalDevice device) const {
        SwapChainInfo swapChainInfo;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vulkan->getSurface(), &swapChainInfo.surfaceCapabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan->getSurface(), &formatCount, nullptr);
        swapChainInfo.surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan->getSurface(), &formatCount, swapChainInfo.surfaceFormats.data());

        uint32_t presentationModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan->getSurface(), &presentationModeCount, nullptr);
        swapChainInfo.presentModes.resize(presentationModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan->getSurface(), &presentationModeCount, swapChainInfo.presentModes.data());

        return swapChainInfo;
    }

    uint32_t VulkanPhysicalDevice::getSuitabilityRating(const VulkanPhysicalDeviceInfo& deviceInfo, const std::vector<const char*>& requiredExtensions) const {
        if (!hasRequiredExtensions(requiredExtensions, deviceInfo.extensions)) {
            BL_LOG_DEBUG("{0} does not have required device extensions", deviceInfo.properties.deviceName);
            return 0;
        }
        if (!hasRequiredQueueFamilyIndices(deviceInfo.queueFamilyIndices)) {
            BL_LOG_DEBUG("{0} does not have required queue family indices", deviceInfo.properties.deviceName);
            return 0;
        }
         if (!hasRequiredSwapChainSupport(deviceInfo.swapChainInfo)) {
            BL_LOG_DEBUG("{0} does not have required swap chain info", deviceInfo.properties.deviceName);
            return 0;
        }
         /*
        if (!hasRequiredFeatures(deviceInfo.features)) {
            BL_LOG_DEBUG("{0} does not have required device features", deviceInfo.properties.deviceName);
            return 0;
        }
         */
        uint32_t rating = deviceInfo.properties.limits.maxImageDimension2D;
        if (deviceInfo.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            rating += 1000;
        }
        return rating;
    }

    bool VulkanPhysicalDevice::hasRequiredExtensions(const std::vector<const char*>& extensions, const std::vector<VkExtensionProperties>& availableExtensions) const {
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