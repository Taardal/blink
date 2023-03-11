#pragma once

#include "AppConfig.h"
#include <vulkan/vulkan.h>
#include <vector>

#define BL_VK_ALLOCATOR nullptr

namespace Blink {

    struct VulkanConfig {
        std::string applicationName;
        std::string engineName;
        std::vector<const char*> validationLayers;
        std::vector<const char*> requiredExtensions;

        bool isValidationLayersEnabled() const {
            return !validationLayers.empty();
        }
    };

    class Vulkan {
    private:
        VkInstance vkInstance = nullptr;
        VkDebugUtilsMessengerEXT debugMessenger = nullptr;
        bool validationLayersEnabled = false;

    public:
        std::vector<VkPhysicalDevice> getPhysicalDevices() const;

        bool initialize(const VulkanConfig& vulkanConfig);

        void terminate();

    private:
        bool createInstance(const VulkanConfig& config);

        void destroyInstance();

        bool createDebugMessenger();

        void destroyDebugMessenger();

        VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo() const;

        bool hasValidationLayers(const std::vector<const char*>& validationLayers) const;

        std::vector<VkLayerProperties> findAvailableValidationLayers() const;

        bool hasExtensions(const std::vector<const char*>& extensions) const;

        std::vector<VkExtensionProperties> findAvailableExtensions() const;
    };
}
