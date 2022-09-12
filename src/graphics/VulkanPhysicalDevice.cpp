#include "pch.h"
#include "VulkanPhysicalDevice.h"

namespace Blink {

    const std::vector<const char*> VulkanPhysicalDevice::requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef BL_PLATFORM_MACOS
            "VK_KHR_portability_subset"
#endif
    };

    VulkanPhysicalDevice::VulkanPhysicalDevice(Vulkan* vulkan)
            : vulkan(vulkan) {}

    const QueueFamilyIndices& VulkanPhysicalDevice::getQueueFamilyIndices() const {
        return deviceInfo.queueFamilyIndices;
    }

    const VkPhysicalDeviceFeatures& VulkanPhysicalDevice::getFeatures() const {
        return deviceInfo.features;
    }

    bool VulkanPhysicalDevice::initialize() {
        std::vector<VkPhysicalDevice> availableDevices = vulkan->getPhysicalDevices();
        if (availableDevices.empty()) {
            BL_LOG_ERROR("Could not find any physical devices");
            return false;
        }
        VulkanPhysicalDeviceInfo deviceInfo = getMostSuitableDevice(availableDevices);
        if (deviceInfo.physicalDevice == nullptr) {
            BL_LOG_ERROR("Could not get any suitable device");
            return false;
        }
        this->deviceInfo = deviceInfo;
        BL_LOG_INFO("Using physical device [{}]", deviceInfo.properties.deviceName);
        return true;
    }

    void VulkanPhysicalDevice::terminate() {

    }

    VkResult VulkanPhysicalDevice::createDevice(const VkDeviceCreateInfo& vkDeviceCreateInfo, VkDevice* vkDevice) const {
        return vkCreateDevice(deviceInfo.physicalDevice, &vkDeviceCreateInfo, BL_VK_ALLOCATOR, vkDevice);
    }

    VulkanPhysicalDeviceInfo VulkanPhysicalDevice::getMostSuitableDevice(const std::vector<VkPhysicalDevice>& vkPhysicalDevices) const {
        std::multimap<uint32_t , VulkanPhysicalDeviceInfo> devicesByRating;
        for (VkPhysicalDevice vkPhysicalDevice : vkPhysicalDevices) {
            VulkanPhysicalDeviceInfo deviceInfo = getDeviceInfo(vkPhysicalDevice);
            uint32_t suitabilityRating = getSuitabilityRating(deviceInfo);
            devicesByRating.insert(std::make_pair(suitabilityRating, deviceInfo));
        }
        uint32_t highestRating = devicesByRating.rbegin()->first;
        if (highestRating == 0) {
            return {};
        }
        return devicesByRating.rbegin()->second;
    }

    VulkanPhysicalDeviceInfo VulkanPhysicalDevice::getDeviceInfo(VkPhysicalDevice vkPhysicalDevice) const {
        VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &vkPhysicalDeviceProperties);

        VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures;
        vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &vkPhysicalDeviceFeatures);

        VulkanPhysicalDeviceInfo deviceInfo{};
        deviceInfo.physicalDevice = vkPhysicalDevice;
        deviceInfo.properties = vkPhysicalDeviceProperties;
        deviceInfo.features = vkPhysicalDeviceFeatures;
        //deviceInfo.extensions = findExtensions(vkPhysicalDevice);
        deviceInfo.queueFamilyIndices = findQueueFamilyIndices(vkPhysicalDevice);
        //deviceInfo.swapChainInfo = findSwapChainInfo(vkPhysicalDevice);

        return deviceInfo;
    }

    uint32_t VulkanPhysicalDevice::getSuitabilityRating(const VulkanPhysicalDeviceInfo& deviceInfo) const {
        /*
        if (!hasRequiredFeatures(deviceInfo.features)) {
            BL_LOG_DEBUG("{0} does not have required device features", deviceInfo.properties.deviceName);
            return 0;
        }
        if (!hasRequiredExtensions(deviceInfo.extensions)) {
            BL_LOG_DEBUG("{0} does not have required device extensions", deviceInfo.properties.deviceName);
            return 0;
        }
        if (!hasRequiredSwapChainSupport(deviceInfo.swapChainInfo)) {
            BL_LOG_DEBUG("{0} does not have required swap chain info", deviceInfo.properties.deviceName);
            return 0;
        }
        */
        if (!hasRequiredQueueFamilyIndices(deviceInfo.queueFamilyIndices)) {
            BL_LOG_DEBUG("{0} does not have required queue family indices", deviceInfo.properties.deviceName);
            return 0;
        }
        uint32_t rating = deviceInfo.properties.limits.maxImageDimension2D;
        if (deviceInfo.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            rating += 1000;
        }
        return rating;
    }

    QueueFamilyIndices VulkanPhysicalDevice::findQueueFamilyIndices(VkPhysicalDevice vkPhysicalDevice) const {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

        QueueFamilyIndices indices;
        for (int i = 0; i < queueFamilies.size(); i++) {
            const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            /*
            VkBool32 presentationSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, vulkan->getSurface(), &presentationSupport);
            if (presentationSupport) {
                indices.presentationFamily = i;
            }
            */
            if (hasRequiredQueueFamilyIndices(indices)) {
                break;
            }
        }
        return indices;
    }

    bool VulkanPhysicalDevice::hasRequiredQueueFamilyIndices(const QueueFamilyIndices& queueFamilyIndices) const {
        return queueFamilyIndices.graphicsFamily.has_value();
    }

    std::vector<VkExtensionProperties> VulkanPhysicalDevice::findRequiredExtensions() const {
        static std::vector<const char*> extensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        return std::vector<VkExtensionProperties>();
    }
}