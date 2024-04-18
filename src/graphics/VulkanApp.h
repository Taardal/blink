#pragma once

#include "window/Window.h"
#include <vulkan/vulkan.h>
#include <vector>

#define BL_VULKAN_ALLOCATOR nullptr

#define BL_ASSERT_VK_SUCCESS(expression) BL_ASSERT(expression == VK_SUCCESS)
#define BL_ASSERT_THROW_VK_SUCCESS(expression) BL_ASSERT_THROW(expression == VK_SUCCESS)

namespace Blink {
    struct VulkanAppConfig {
        Window* window = nullptr;
        std::string applicationName;
        std::string engineName;
        bool validationLayersEnabled = false;
    };

    class VulkanApp {
    private:
        VulkanAppConfig config;
        VkInstance instance = nullptr;
        VkDebugUtilsMessengerEXT debugMessenger = nullptr;
        VkSurfaceKHR surface = nullptr;

    public:
        VulkanApp(const VulkanAppConfig& config);

        ~VulkanApp();

        std::vector<VkPhysicalDevice> getPhysicalDevices() const;

        VkSurfaceKHR getSurface() const;

    private:
        void createInstance(
            const std::vector<const char*>& requiredExtensions,
            const std::vector<const char*>& validationLayers,
            const VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo
        );

        void destroyInstance() const;

        void createDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo);

        void destroyDebugMessenger() const;

        void createSurface();

        void destroySurface() const;

        bool hasValidationLayers(const std::vector<const char*>& validationLayers) const;

        std::vector<VkLayerProperties> getAvailableValidationLayers() const;

        bool hasExtensions(const std::vector<const char*>& extensions) const;

        std::vector<VkExtensionProperties> getAvailableExtensions() const;

        VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo() const;
    };
}
