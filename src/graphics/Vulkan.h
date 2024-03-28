#pragma once

#include "AppConfig.h"
#include "window/Window.h"
#include "system/Log.h"
#include <vulkan/vulkan.h>
#include <vector>

#define BL_VULKAN_ALLOCATOR VK_NULL_HANDLE

#define BL_VULKAN_ASSERT(expression) BL_ASSERT(expression == VK_SUCCESS)
#define BL_VULKAN_ASSERT_THROW(expression) BL_ASSERT_THROW(expression == VK_SUCCESS)

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
        Vulkan(const VulkanConfig& vulkanConfig, Window* window);

        ~Vulkan();

        std::vector<VkPhysicalDevice> getPhysicalDevices() const;

        VkSurfaceKHR getSurface() const;

    private:
        bool createInstance(
            const VulkanConfig& vulkanConfig,
            const std::vector<const char*>& requiredExtensions,
            const std::vector<const char*>& validationLayers,
            const VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo
        );

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
