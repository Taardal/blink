#include "pch.h"
#include "VulkanDevice.h"

namespace Blink {

    VulkanDevice::VulkanDevice(VulkanPhysicalDevice* physicalDevice)
            : physicalDevice(physicalDevice) {}

    bool VulkanDevice::initialize() {
        const QueueFamilyIndices& queueFamilyIndices = physicalDevice->getQueueFamilyIndices();
        if (!createDevice(queueFamilyIndices)) {
            BL_LOG_ERROR("Could not create logical device");
            return false;
        }
        BL_LOG_INFO("Created logical device");
        this->graphicsQueue = getDeviceQueue(queueFamilyIndices.graphicsFamily.value());
        if (graphicsQueue == nullptr) {
            BL_LOG_ERROR("Could not get graphics queue");
            return false;
        }
        this->presentQueue = getDeviceQueue(queueFamilyIndices.presentFamily.value());
        if (presentQueue == nullptr) {
            BL_LOG_ERROR("Could not get present queue");
            return false;
        }
        return true;
    }

    void VulkanDevice::terminate() const {
        destroyDevice();
        BL_LOG_INFO("Destroyed logical device");
    }

    VkResult VulkanDevice::createSwapChain(VkSwapchainCreateInfoKHR* createInfo, VkSwapchainKHR* swapchain) const {
        return vkCreateSwapchainKHR(device, createInfo, BL_VK_ALLOCATOR, swapchain);
    }

    void VulkanDevice::destroySwapChain(VkSwapchainKHR swapChain) const {
        vkDestroySwapchainKHR(device, swapChain, BL_VK_ALLOCATOR);
    }

    void VulkanDevice::getSwapChainImages(uint32_t* imageCount, std::vector<VkImage>* images, VkSwapchainKHR swapChain) const {
        vkGetSwapchainImagesKHR(device, swapChain, imageCount, nullptr);
        images->resize(*imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, imageCount, images->data());
    }

    VkResult VulkanDevice::createImageView(VkImageViewCreateInfo* createInfo, VkImageView* imageView) const {
        return vkCreateImageView(device, createInfo, BL_VK_ALLOCATOR, imageView);
    }

    void VulkanDevice::destroyImageView(VkImageView imageView) const {
        vkDestroyImageView(device, imageView, BL_VK_ALLOCATOR);
    }

    bool VulkanDevice::createDevice(const QueueFamilyIndices& queueFamilyIndices) {
        const VkPhysicalDeviceFeatures features = physicalDevice->getFeatures();

        std::vector<const char*> extensionNames;
        for (const VkExtensionProperties& extension : physicalDevice->getExtensions()) {
            extensionNames.push_back(extension.extensionName);
        }

        // Vulkan lets you assign priorities to queues to influence the scheduling of command buffer execution using floating point numbers between 0.0 and 1.0.
        // This is required even if there is only a single queue
        float queuePriority = 1.0f;
        std::set<uint32_t> queueFamilies = {
                queueFamilyIndices.graphicsFamily.value(),
                queueFamilyIndices.presentFamily.value()
        };
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        for (uint32_t queueFamily : queueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pEnabledFeatures = &features;
        createInfo.enabledExtensionCount = (uint32_t) extensionNames.size();
        createInfo.ppEnabledExtensionNames = extensionNames.data();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = (uint32_t) queueCreateInfos.size();

        return physicalDevice->createDevice(&createInfo, &device) == VK_SUCCESS;
    }

    void VulkanDevice::destroyDevice() const {
        vkDestroyDevice(device, BL_VK_ALLOCATOR);
    }

    VkQueue VulkanDevice::getDeviceQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const {
        VkQueue queue;
        vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue);
        return queue;
    }
}