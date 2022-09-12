#include "VulkanDevice.h"

namespace Blink {

    VulkanDevice::VulkanDevice(VulkanPhysicalDevice* physicalDevice)
            : physicalDevice(physicalDevice) {}

    bool VulkanDevice::initialize(const Config& config) {
        const QueueFamilyIndices& queueFamilyIndices = physicalDevice->getQueueFamilyIndices();

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;

        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;

        const VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures = physicalDevice->getFeatures();
        createInfo.pEnabledFeatures = &vkPhysicalDeviceFeatures;

        if (config.vulkanValidationLayersEnabled) {
            createInfo.enabledLayerCount = (uint32_t) Vulkan::validationLayers.size();
            createInfo.ppEnabledLayerNames = Vulkan::validationLayers.data();
        }
        createInfo.enabledExtensionCount = (uint32_t) VulkanPhysicalDevice::requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = VulkanPhysicalDevice::requiredExtensions.data();

        if (physicalDevice->createDevice(createInfo, &vkDevice) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create logical device");
            return false;
        }
        BL_LOG_INFO("Created logical device");

        constexpr uint32_t graphicsQueueIndex = 0; // Because we're only creating a single queue from this family, we'll simply use index 0
        VkQueue graphicsVkQueue = findDeviceQueue(queueFamilyIndices.graphicsFamily.value(), graphicsQueueIndex);
        if (graphicsVkQueue == nullptr) {
            BL_LOG_ERROR("Could not get graphics queue");
            return false;
        }
        this->graphicsVkQueue = graphicsVkQueue;

        return true;
    }

    void VulkanDevice::terminate() const {
        vkDestroyDevice(vkDevice, BL_VK_ALLOCATOR);
        BL_LOG_INFO("Destroyed logical device");
    }

    VkQueue VulkanDevice::findDeviceQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const {
        VkQueue queue;
        vkGetDeviceQueue(vkDevice, queueFamilyIndex, queueIndex, &queue);
        return queue;
    }
}