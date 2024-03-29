#include "pch.h"
#include "VulkanDevice.h"

namespace Blink {

    VulkanDevice::VulkanDevice(VulkanPhysicalDevice* physicalDevice) : physicalDevice(physicalDevice) {
        const QueueFamilyIndices& queueFamilyIndices = physicalDevice->getQueueFamilyIndices();
        if (!createDevice(queueFamilyIndices)) {
            throw std::runtime_error("Could not create logical device");
        }
        BL_LOG_INFO("Created logical device");
        this->graphicsQueue = getDeviceQueue(queueFamilyIndices.graphicsFamily.value());
        if (graphicsQueue == nullptr) {
            throw std::runtime_error("Could not get graphics queue");
        }
        this->presentQueue = getDeviceQueue(queueFamilyIndices.presentFamily.value());
        if (presentQueue == nullptr) {
            throw std::runtime_error("Could not get present queue");
        }
    }

    VulkanDevice::~VulkanDevice() {
        destroyDevice();
        BL_LOG_INFO("Destroyed logical device");
    }

    VulkanDevice::operator VkDevice() const {
        return device;
    }

    VkQueue VulkanDevice::getGraphicsQueue() const {
        return graphicsQueue;
    }

    VkQueue VulkanDevice::getPresentQueue() const {
        return presentQueue;
    }

    void VulkanDevice::waitUntilIdle() const {
        vkDeviceWaitIdle(device);
    }

    VkResult VulkanDevice::createSwapChain(VkSwapchainCreateInfoKHR* createInfo, VkSwapchainKHR* swapchain) const {
        return vkCreateSwapchainKHR(device, createInfo, BL_VULKAN_ALLOCATOR, swapchain);
    }

    void VulkanDevice::destroySwapChain(VkSwapchainKHR swapChain) const {
        vkDestroySwapchainKHR(device, swapChain, BL_VULKAN_ALLOCATOR);
    }

    void VulkanDevice::getSwapChainImages(uint32_t* imageCount, std::vector<VkImage>* images, VkSwapchainKHR swapChain) const {
        vkGetSwapchainImagesKHR(device, swapChain, imageCount, nullptr);
        images->resize(*imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, imageCount, images->data());
    }

    VkResult VulkanDevice::createImageView(VkImageViewCreateInfo* createInfo, VkImageView* imageView) const {
        return vkCreateImageView(device, createInfo, BL_VULKAN_ALLOCATOR, imageView);
    }

    void VulkanDevice::destroyImageView(VkImageView imageView) const {
        vkDestroyImageView(device, imageView, BL_VULKAN_ALLOCATOR);
    }

    VkResult VulkanDevice::createShaderModule(VkShaderModuleCreateInfo* createInfo, VkShaderModule* shaderModule) const {
        return vkCreateShaderModule(device, createInfo, BL_VULKAN_ALLOCATOR, shaderModule);
    }

    void VulkanDevice::destroyShaderModule(VkShaderModule shaderModule) const {
        vkDestroyShaderModule(device, shaderModule, BL_VULKAN_ALLOCATOR);
    }

    VkResult VulkanDevice::createPipelineLayout(VkPipelineLayoutCreateInfo* createInfo, VkPipelineLayout* layout) const {
        return vkCreatePipelineLayout(device, createInfo, BL_VULKAN_ALLOCATOR, layout);
    }

    void VulkanDevice::destroyPipelineLayout(VkPipelineLayout layout) const {
        vkDestroyPipelineLayout(device, layout, BL_VULKAN_ALLOCATOR);
    }

    VkResult VulkanDevice::createRenderPass(VkRenderPassCreateInfo* createInfo, VkRenderPass* renderPass) const {
        return vkCreateRenderPass(device, createInfo, BL_VULKAN_ALLOCATOR, renderPass);
    }

    void VulkanDevice::destroyRenderPass(VkRenderPass renderPass) const {
        vkDestroyRenderPass(device, renderPass, BL_VULKAN_ALLOCATOR);
    }

    VkResult VulkanDevice::createGraphicsPipeline(VkGraphicsPipelineCreateInfo* createInfo, VkPipeline* pipeline) const {
        constexpr uint32_t count = 1;
        VkPipelineCache cache = VK_NULL_HANDLE;
        return vkCreateGraphicsPipelines(device, cache, count, createInfo, BL_VULKAN_ALLOCATOR, pipeline);
    };

    void VulkanDevice::destroyGraphicsPipeline(VkPipeline pipeline) const {
        vkDestroyPipeline(device, pipeline, BL_VULKAN_ALLOCATOR);
    };

    VkResult VulkanDevice::createFramebuffer(VkFramebufferCreateInfo* createInfo, VkFramebuffer* framebuffer) const {
        return vkCreateFramebuffer(device, createInfo, BL_VULKAN_ALLOCATOR, framebuffer);
    };

    void VulkanDevice::destroyFramebuffer(VkFramebuffer framebuffer) const {
        vkDestroyFramebuffer(device, framebuffer, BL_VULKAN_ALLOCATOR);
    };

    VkResult VulkanDevice::createCommandPool(VkCommandPoolCreateInfo* createInfo, VkCommandPool* commandPool) const {
        return vkCreateCommandPool(device, createInfo, BL_VULKAN_ALLOCATOR, commandPool);
    };

    void VulkanDevice::destroyCommandPool(VkCommandPool commandPool) const {
        vkDestroyCommandPool(device, commandPool, BL_VULKAN_ALLOCATOR);
    };

    VkResult VulkanDevice::allocateCommandBuffers(VkCommandBufferAllocateInfo* allocateInfo, VkCommandBuffer* commandBuffers) const {
        return vkAllocateCommandBuffers(device, allocateInfo, commandBuffers);
    };

    void VulkanDevice::freeCommandBuffers(uint32_t count, VkCommandBuffer* commandBuffers, VkCommandPool commandPool) const {
        vkFreeCommandBuffers(device, commandPool, count, commandBuffers);
    };

    VkResult VulkanDevice::createSemaphore(VkSemaphoreCreateInfo* createInfo, VkSemaphore* semaphore) const {
        return vkCreateSemaphore(device, createInfo, BL_VULKAN_ALLOCATOR, semaphore);
    };

    void VulkanDevice::destroySemaphore(VkSemaphore semaphore) const {
        vkDestroySemaphore(device, semaphore, BL_VULKAN_ALLOCATOR);
    };

    VkResult VulkanDevice::createFence(VkFenceCreateInfo* createInfo, VkFence* fence) const {
        return vkCreateFence(device, createInfo, BL_VULKAN_ALLOCATOR, fence);
    };

    void VulkanDevice::destroyFence(VkFence fence) const {
        vkDestroyFence(device, fence, BL_VULKAN_ALLOCATOR);
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

    VkResult VulkanDevice::createBuffer(VkBufferCreateInfo* createInfo, VkBuffer* buffer) const {
        return vkCreateBuffer(device, createInfo, BL_VULKAN_ALLOCATOR, buffer);
    }

    void VulkanDevice::destroyBuffer(VkBuffer buffer) const {
        vkDestroyBuffer(device, buffer, BL_VULKAN_ALLOCATOR);
    }

    VkMemoryRequirements VulkanDevice::getBufferMemoryRequirements(VkBuffer buffer) const {
        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);
        return memoryRequirements;
    }

    VkResult VulkanDevice::allocateMemory(VkMemoryAllocateInfo* allocateInfo, VkDeviceMemory* memory) const {
        return vkAllocateMemory(device, allocateInfo, BL_VULKAN_ALLOCATOR, memory);
    }

    void VulkanDevice::freeMemory(VkDeviceMemory memory) const {
        vkFreeMemory(device, memory, BL_VULKAN_ALLOCATOR);
    }

    void VulkanDevice::bindBufferMemory(VkBuffer buffer, VkDeviceMemory memory) const {
        constexpr VkDeviceSize memoryOffset = 0;
        vkBindBufferMemory(device, buffer, memory, memoryOffset);
    }

    void VulkanDevice::mapMemory(VkDeviceMemory memory, VkDeviceSize memorySize, void** data) const {
        constexpr VkDeviceSize memoryOffset = 0;
        constexpr VkMemoryMapFlags memoryMapFlags = 0;
        vkMapMemory(device, memory, memoryOffset, memorySize, memoryMapFlags, data);
    }

    void VulkanDevice::unmapMemory(VkDeviceMemory memory) const {
        vkUnmapMemory(device, memory);
    }

    VkResult VulkanDevice::createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* createInfo, VkDescriptorSetLayout* layout) const {
        return vkCreateDescriptorSetLayout(device, createInfo, BL_VULKAN_ALLOCATOR, layout);
    }

    void VulkanDevice::destroyDescriptorSetLayout(VkDescriptorSetLayout layout) const {
        vkDestroyDescriptorSetLayout(device, layout, BL_VULKAN_ALLOCATOR);
    }

    VkResult VulkanDevice::createDescriptorPool(VkDescriptorPoolCreateInfo* createInfo, VkDescriptorPool* pool) const {
        return vkCreateDescriptorPool(device, createInfo, BL_VULKAN_ALLOCATOR, pool);
    }

    void VulkanDevice::destroyDescriptorPool(VkDescriptorPool pool) const {
        vkDestroyDescriptorPool(device, pool, BL_VULKAN_ALLOCATOR);
    }

    VkResult VulkanDevice::allocateDescriptorSets(VkDescriptorSetAllocateInfo* allocateInfo, VkDescriptorSet* descriptorSets) const {
        return vkAllocateDescriptorSets(device, allocateInfo, descriptorSets);
    }

    void VulkanDevice::updateDescriptorSets(uint32_t count, VkWriteDescriptorSet* write) const {
        constexpr uint32_t copyCount = 0;
        constexpr VkCopyDescriptorSet* copies = nullptr;
        vkUpdateDescriptorSets(device, count, write, copyCount, copies);
    }

    VkResult VulkanDevice::createImage(VkImageCreateInfo* createInfo, VkImage* image) const {
        return vkCreateImage(device, createInfo, BL_VULKAN_ALLOCATOR, image);
    }

    VkMemoryRequirements VulkanDevice::getImageMemoryRequirements(VkImage image) const {
        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device, image, &memoryRequirements);
        return memoryRequirements;
    }

    void VulkanDevice::bindImageMemory(VkImage image, VkDeviceMemory memory) const {
        constexpr VkDeviceSize memoryOffset = 0;
        vkBindImageMemory(device, image, memory, memoryOffset);
    }

    void VulkanDevice::destroyImage(VkImage image) const {
        vkDestroyImage(device, image, BL_VULKAN_ALLOCATOR);
    }

    VkResult VulkanDevice::createSampler(VkSamplerCreateInfo* createInfo, VkSampler* sampler) const {
        return vkCreateSampler(device, createInfo, BL_VULKAN_ALLOCATOR, sampler);
    }

    void VulkanDevice::destroySampler(VkSampler sampler) const {
        vkDestroySampler(device, sampler, BL_VULKAN_ALLOCATOR);
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
        vkDestroyDevice(device, BL_VULKAN_ALLOCATOR);
    }

    VkQueue VulkanDevice::getDeviceQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const {
        VkQueue queue;
        vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, &queue);
        return queue;
    }
}