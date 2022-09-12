#pragma once

#include "Config.h"
#include <vulkan/vulkan.h>

#define BL_VK_ALLOCATOR nullptr

namespace Blink {
    class Vulkan {
    public:
        static const std::vector<const char*> validationLayers;

    private:
        VkInstance vkInstance = nullptr;
        VkDebugUtilsMessengerEXT vkDebugMessenger = nullptr;
        bool validationLayersEnabled = false;

    public:
        bool initialize(const Config& config);

        void terminate();

        std::vector<VkPhysicalDevice> getPhysicalDevices() const;

    private:
        std::vector<VkLayerProperties> findAvailableValidationLayers() const;

        bool hasValidationLayers(const std::vector<const char*>& validationLayers) const;

        std::vector<const char*> findRequiredExtensions() const;

        bool hasExtensions(const std::vector<const char*>& extensions) const;

        std::vector<VkExtensionProperties> findAvailableExtensions() const;

        VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo() const;

        bool createDebugMessenger();

        void destroyDebugMessenger();
    };
}
