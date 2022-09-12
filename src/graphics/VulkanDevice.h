#pragma once

#include "Config.h"
#include "VulkanPhysicalDevice.h"
#include <vulkan/vulkan.h>

namespace Blink {
    class VulkanDevice {
    private:
        VulkanPhysicalDevice* physicalDevice;
        VkDevice vkDevice = nullptr;
        VkQueue graphicsVkQueue = nullptr;

    public:
        explicit VulkanDevice(VulkanPhysicalDevice* physicalDevice);

        bool initialize(const Config& config);

        void terminate() const;

    private:
        VkQueue findDeviceQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const;
    };
}
