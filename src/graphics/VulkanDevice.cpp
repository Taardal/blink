#include "VulkanDevice.h"

namespace Blink {

    VulkanDevice::VulkanDevice(VulkanPhysicalDevice* physicalDevice)
            : physicalDevice(physicalDevice) {}

    bool VulkanDevice::initialize(const AppConfig& config) {
        const QueueFamilyIndices& queueFamilyIndices = physicalDevice->getQueueFamilyIndices();
        if (!createDevice(queueFamilyIndices)) {
            BL_LOG_ERROR("Could not create logical device");
            return false;
        }
        BL_LOG_INFO("Created logical device");
        this->graphicsQueue = getGraphicsQueue(queueFamilyIndices);
        if (graphicsQueue == nullptr) {
            BL_LOG_ERROR("Could not get graphics queue");
            return false;
        }
        return true;
    }

    void VulkanDevice::terminate() const {
        destroyDevice();
        BL_LOG_INFO("Destroyed logical device");
    }

    bool VulkanDevice::createDevice(const QueueFamilyIndices& queueFamilyIndices) {
        const VkPhysicalDeviceFeatures features = physicalDevice->getFeatures();

        std::vector<const char*> extensionNames;
        for (const VkExtensionProperties& extension : physicalDevice->getExtensions()) {
            extensionNames.push_back(extension.extensionName);
        }

        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &features;
        createInfo.enabledExtensionCount = (uint32_t) extensionNames.size();
        createInfo.ppEnabledExtensionNames = extensionNames.data();

        return physicalDevice->createDevice(createInfo, &device) == VK_SUCCESS;
    }

    void VulkanDevice::destroyDevice() const {
        vkDestroyDevice(device, BL_VK_ALLOCATOR);
    }

    VkQueue VulkanDevice::getGraphicsQueue(const QueueFamilyIndices& queueFamilyIndices) const {
        // Because we're only creating a single queue from this family, we'll simply use index 0
        constexpr uint32_t graphicsQueueIndex = 0;
        return findQueue(queueFamilyIndices.graphicsFamily.value(), graphicsQueueIndex);
    }

    VkQueue VulkanDevice::findQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const {
        VkQueue queue;
        vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue);
        return queue;
    }
}