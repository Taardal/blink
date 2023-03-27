#include "pch.h"
#include "VulkanDevice.h"

namespace Blink {

    VulkanDevice::VulkanDevice(VulkanPhysicalDevice* physicalDevice)
            : physicalDevice(physicalDevice) {}

    VkQueue VulkanDevice::getGraphicsQueue() const {
        return graphicsQueue;
    }

    VkQueue VulkanDevice::getPresentQueue() const {
        return presentQueue;
    }

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

    void VulkanDevice::waitUntilIdle() const {
        vkDeviceWaitIdle(device);
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

    VkResult VulkanDevice::createShaderModule(VkShaderModuleCreateInfo* createInfo, VkShaderModule* shaderModule) const {
        return vkCreateShaderModule(device, createInfo, BL_VK_ALLOCATOR, shaderModule);
    }

    void VulkanDevice::destroyShaderModule(VkShaderModule shaderModule) const {
        vkDestroyShaderModule(device, shaderModule, BL_VK_ALLOCATOR);
    }

    VkResult VulkanDevice::createPipelineLayout(VkPipelineLayoutCreateInfo* createInfo, VkPipelineLayout* layout) const {
        return vkCreatePipelineLayout(device, createInfo, BL_VK_ALLOCATOR, layout);
    }

    void VulkanDevice::destroyPipelineLayout(VkPipelineLayout layout) const {
        vkDestroyPipelineLayout(device, layout, BL_VK_ALLOCATOR);
    }

    VkResult VulkanDevice::createRenderPass(VkRenderPassCreateInfo* createInfo, VkRenderPass* renderPass) const {
        return vkCreateRenderPass(device, createInfo, BL_VK_ALLOCATOR, renderPass);
    }

    void VulkanDevice::destroyRenderPass(VkRenderPass renderPass) const {
        vkDestroyRenderPass(device, renderPass, BL_VK_ALLOCATOR);
    }

    VkResult VulkanDevice::createGraphicsPipeline(VkGraphicsPipelineCreateInfo* createInfo, VkPipeline* pipeline) const {
        constexpr uint32_t count = 1;
        VkPipelineCache cache = VK_NULL_HANDLE;
        return vkCreateGraphicsPipelines(device, cache, count, createInfo, BL_VK_ALLOCATOR, pipeline);
    };

    void VulkanDevice::destroyGraphicsPipeline(VkPipeline pipeline) const {
        vkDestroyPipeline(device, pipeline, BL_VK_ALLOCATOR);
    };

    VkResult VulkanDevice::createFramebuffer(VkFramebufferCreateInfo* createInfo, VkFramebuffer* framebuffer) const {
        return vkCreateFramebuffer(device, createInfo, BL_VK_ALLOCATOR, framebuffer);
    };

    void VulkanDevice::destroyFramebuffer(VkFramebuffer framebuffer) const {
        vkDestroyFramebuffer(device, framebuffer, BL_VK_ALLOCATOR);
    };

    VkResult VulkanDevice::createCommandPool(VkCommandPoolCreateInfo* createInfo, VkCommandPool* commandPool) const {
        return vkCreateCommandPool(device, createInfo, BL_VK_ALLOCATOR, commandPool);
    };

    void VulkanDevice::destroyCommandPool(VkCommandPool commandPool) const {
        vkDestroyCommandPool(device, commandPool, BL_VK_ALLOCATOR);
    };

    VkResult VulkanDevice::allocateCommandBuffers(VkCommandBufferAllocateInfo* allocateInfo, VkCommandBuffer* commandBuffers) const {
        return vkAllocateCommandBuffers(device, allocateInfo, commandBuffers);
    };

    void VulkanDevice::freeCommandBuffers(uint32_t count, VkCommandBuffer* commandBuffers, VkCommandPool commandPool) const {
        vkFreeCommandBuffers(device, commandPool, count, commandBuffers);
    };

    VkResult VulkanDevice::createSemaphore(VkSemaphoreCreateInfo* createInfo, VkSemaphore* semaphore) const {
        return vkCreateSemaphore(device, createInfo, BL_VK_ALLOCATOR, semaphore);
    };

    void VulkanDevice::destroySemaphore(VkSemaphore semaphore) const {
        vkDestroySemaphore(device, semaphore, BL_VK_ALLOCATOR);
    };

    VkResult VulkanDevice::createFence(VkFenceCreateInfo* createInfo, VkFence* fence) const {
        return vkCreateFence(device, createInfo, BL_VK_ALLOCATOR, fence);
    };

    void VulkanDevice::destroyFence(VkFence fence) const {
        vkDestroyFence(device, fence, BL_VK_ALLOCATOR);
    };

    void VulkanDevice::waitForFence(VkFence* fence) const {
        uint32_t count = 1;
        bool waitAll = VK_TRUE;
        uint64_t timeout = UINT64_MAX;
        vkWaitForFences(device, count, fence, waitAll, timeout);
    };

    void VulkanDevice::resetFence(VkFence* fence) const {
        uint32_t count = 1;
        resetFences(1, fence);
    };

    void VulkanDevice::resetFences(uint32_t count, VkFence* fences) const {
        vkResetFences(device, count, fences);
    };

    VkResult VulkanDevice::acquireSwapChainImage(VkSwapchainKHR swapChain, VkSemaphore semaphore, uint32_t* imageIndex) const {
        uint64_t timeout = UINT64_MAX;
        VkFence fence = VK_NULL_HANDLE;
        return vkAcquireNextImageKHR(device, swapChain, timeout, semaphore, fence, imageIndex);
    };

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