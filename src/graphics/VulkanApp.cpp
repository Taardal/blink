#include "VulkanApp.h"

namespace Blink {

    static VKAPI_ATTR VkBool32 VKAPI_CALL onDebugMessage(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
    ) {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            BL_LOG_ERROR(pCallbackData->pMessage);
        } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            BL_LOG_WARN(pCallbackData->pMessage);
        } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
            BL_LOG_INFO(pCallbackData->pMessage);
        } else {
            BL_LOG_TRACE(pCallbackData->pMessage);
        }
        return VK_FALSE;
    }
}

namespace Blink {

    VulkanApp::VulkanApp(const VulkanAppConfig& config) : config(config) {
        BL_ASSERT_THROW(config.window->isVulkanSupported());

        std::vector<const char*> requiredExtensions = config.window->getRequiredVulkanExtensions();
        BL_ASSERT_THROW(std::count(requiredExtensions.begin(), requiredExtensions.end(), "VK_KHR_surface") > 0);

#ifdef BL_PLATFORM_MACOS
        requiredExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
        if (config.validationLayersEnabled) {
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        BL_ASSERT_THROW(hasExtensions(requiredExtensions));

        std::vector<const char*> validationLayers;
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        if (config.validationLayersEnabled) {
            validationLayers.push_back("VK_LAYER_KHRONOS_validation");
            BL_ASSERT_THROW(hasValidationLayers(validationLayers));
            debugMessengerCreateInfo = getDebugMessengerCreateInfo();
        }

        createInstance(requiredExtensions, validationLayers, debugMessengerCreateInfo);
        if (config.validationLayersEnabled) {
            createDebugMessenger(debugMessengerCreateInfo);
        }
        createSurface();
    }

    VulkanApp::~VulkanApp() {
        destroySurface();
        if (config.validationLayersEnabled) {
            destroyDebugMessenger();
        }
        destroyInstance();
    }

    std::vector<VkPhysicalDevice> VulkanApp::getPhysicalDevices() const {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        return devices;
    }

    VkSurfaceKHR VulkanApp::getSurface() const {
        return surface;
    }

    void VulkanApp::createInstance(
        const std::vector<const char*>& requiredExtensions,
        const std::vector<const char*>& validationLayers,
        const VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo
    ) {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = config.applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = config.engineName.c_str();
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        if (Environment::isMacOS()) {
            createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }
        if (config.validationLayersEnabled) {
            createInfo.pNext = &debugMessengerCreateInfo;
            createInfo.enabledLayerCount = (uint32_t) validationLayers.size();
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }

        BL_ASSERT_VK_SUCCESS(vkCreateInstance(&createInfo, BL_VULKAN_ALLOCATOR, &instance));
        BL_LOG_INFO("Created instance");
    }

    void VulkanApp::destroyInstance() const {
        vkDestroyInstance(instance, BL_VULKAN_ALLOCATOR);
        BL_LOG_INFO("Destroyed instance");
    }

    void VulkanApp::createDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo) {
        auto createDebugMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        BL_ASSERT_THROW(createDebugMessenger != nullptr);
        BL_ASSERT_VK_SUCCESS(createDebugMessenger(instance, &debugMessengerCreateInfo, BL_VULKAN_ALLOCATOR, &debugMessenger));
        BL_LOG_INFO("Created debug messenger");
    }

    void VulkanApp::destroyDebugMessenger() const {
        auto destroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (destroyDebugMessenger == nullptr) {
            return;
        }
        destroyDebugMessenger(instance, debugMessenger, BL_VULKAN_ALLOCATOR);
        BL_LOG_INFO("Destroyed debug messenger");
    }

    void VulkanApp::createSurface() {
        BL_ASSERT_VK_SUCCESS(config.window->createVulkanSurface(instance, &surface, BL_VULKAN_ALLOCATOR));
        BL_LOG_INFO("Created surface");
    }

    void VulkanApp::destroySurface() const {
        vkDestroySurfaceKHR(instance, surface, BL_VULKAN_ALLOCATOR);
        BL_LOG_INFO("Destroyed surface");
    }

    bool VulkanApp::hasValidationLayers(const std::vector<const char*>& validationLayers) const {
        std::vector<VkLayerProperties> availableValidationLayers = getAvailableValidationLayers();
        for (const char* layerName: validationLayers) {
            bool layerFound = false;
            for (const auto& availableLayer: availableValidationLayers) {
                if (strcmp(layerName, availableLayer.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) {
                BL_LOG_ERROR("Could not find validation layer [{0}]", layerName);
                return false;
            }
        }
        return true;
    }

    std::vector<VkLayerProperties> VulkanApp::getAvailableValidationLayers() const {
        uint32_t count;
        VkLayerProperties* layerProperties = nullptr;
        vkEnumerateInstanceLayerProperties(&count, layerProperties);
        std::vector<VkLayerProperties> layers(count);
        vkEnumerateInstanceLayerProperties(&count, layers.data());
        return layers;
    }

    bool VulkanApp::hasExtensions(const std::vector<const char*>& extensions) const {
        std::vector<VkExtensionProperties> availableExtensions = getAvailableExtensions();
        for (const char* extension: extensions) {
            bool extensionFound = false;
            for (const VkExtensionProperties& availableExtension: availableExtensions) {
                if (strcmp(extension, availableExtension.extensionName) == 0) {
                    extensionFound = true;
                    break;
                }
            }
            if (!extensionFound) {
                BL_LOG_ERROR("Could not find extension [{0}]", extension);
                return false;
            }
        }
        return true;
    }

    std::vector<VkExtensionProperties> VulkanApp::getAvailableExtensions() const {
        uint32_t extensionCount = 0;
        const char* layerName = nullptr;
        VkExtensionProperties* extensionProperties = nullptr;
        vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, extensionProperties);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, extensions.data());
        return extensions;
    }

    VkDebugUtilsMessengerCreateInfoEXT VulkanApp::getDebugMessengerCreateInfo() const {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                     | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                     | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        createInfo.pfnUserCallback = onDebugMessage;
        return createInfo;
    }
}
