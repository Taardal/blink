#pragma once

#include "AppConfig.h"
#include "VulkanPhysicalDevice.h"
#include <vulkan/vulkan.h>

namespace Blink {

    class VulkanDevice {
    private:
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VkDevice device = nullptr;
        VkQueue graphicsQueue = nullptr;

    public:
        explicit VulkanDevice(VulkanPhysicalDevice* physicalDevice);

        bool initialize(const AppConfig& config);

        void terminate() const;

    private:
        bool createDevice(const QueueFamilyIndices& queueFamilyIndices);

        void destroyDevice() const;

        VkQueue getGraphicsQueue(const QueueFamilyIndices& queueFamilyIndices) const;

        VkQueue findQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const;
    };
}
