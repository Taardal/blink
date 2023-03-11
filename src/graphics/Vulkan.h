#pragma once

#include "AppConfig.h"
#include "window/Window.h"
#include <vulkan/vulkan.h>
#include <vector>

#define BL_VK_ALLOCATOR nullptr

namespace Blink {

    struct VulkanConfig {
        std::string applicationName;
        std::string engineName;
        bool validationLayersEnabled = false;
    };

    class Vulkan {
    private:
        Window* window = nullptr;
        VkInstance vulkanInstance = nullptr;
        VkDebugUtilsMessengerEXT debugMessenger = nullptr;
        VkSurfaceKHR surface = nullptr;
        bool validationLayersEnabled = false;

    public:
        explicit Vulkan(Window* window);

        std::vector<VkPhysicalDevice> getPhysicalDevices() const;

        VkSurfaceKHR getSurface() const;

        bool initialize(const VulkanConfig& vulkanConfig);

        void terminate();

    private:
        bool createInstance(const VulkanConfig& vulkanConfig, const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& validationLayers, const VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo);

        void destroyInstance();

        bool createDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo);

        void destroyDebugMessenger();

        bool createSurface();

        void destroySurface() const;

        bool hasValidationLayers(const std::vector<const char*>& validationLayers) const;

        std::vector<VkLayerProperties> getAvailableValidationLayers() const;

        bool hasExtensions(const std::vector<const char*>& extensions) const;

        std::vector<VkExtensionProperties> getAvailableExtensions() const;

        VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo() const;
    };
}
