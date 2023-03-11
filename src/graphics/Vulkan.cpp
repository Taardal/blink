#include "Vulkan.h"
#include <GLFW/glfw3.h>

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

    std::vector<VkPhysicalDevice> Vulkan::getPhysicalDevices() const {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
        if (!deviceCount) {
            return {};
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());
        return devices;
    }

    bool Vulkan::initialize(const VulkanConfig& vulkanConfig) {
        validationLayersEnabled = vulkanConfig.isValidationLayersEnabled();
        if (validationLayersEnabled && !hasValidationLayers(vulkanConfig.validationLayers)) {
            BL_LOG_ERROR("Could not find requested validation layers");
            return false;
        }
        if (!hasExtensions(vulkanConfig.requiredExtensions)) {
            BL_LOG_ERROR("Could not find required extensions");
            return false;
        }
        if (!createInstance(vulkanConfig)) {
            BL_LOG_ERROR("Could not create Vulkan instance");
            return false;
        }
        BL_LOG_INFO("Created Vulkan instance");
        if (validationLayersEnabled) {
            createDebugMessenger();
        }
        return true;
    }

    void Vulkan::terminate() {
        if (validationLayersEnabled) {
            destroyDebugMessenger();
        }
        destroyInstance();
        BL_LOG_INFO("Destroyed Vulkan instance");
    }

    bool Vulkan::createInstance(const VulkanConfig& config) {
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
        createInfo.enabledExtensionCount = (uint32_t) config.requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = config.requiredExtensions.data();
        if (Environment::isMacOS()) {
            createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        if (validationLayersEnabled) {
            debugMessengerCreateInfo = getDebugMessengerCreateInfo();
            createInfo.pNext = &debugMessengerCreateInfo;
            createInfo.enabledLayerCount = (uint32_t) config.validationLayers.size();
            createInfo.ppEnabledLayerNames = config.validationLayers.data();
        }

        return vkCreateInstance(&createInfo, BL_VK_ALLOCATOR, &vkInstance) == VK_SUCCESS;
    }

    void Vulkan::destroyInstance() {
        vkDestroyInstance(vkInstance, BL_VK_ALLOCATOR);
    }

    bool Vulkan::createDebugMessenger() {
        const char* functionName = "vkCreateDebugUtilsMessengerEXT";
        auto function = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, functionName);
        if (function == nullptr) {
            BL_LOG_ERROR("Could not look up address of extension function [{0}]", functionName);
            return false;
        }
        VkDebugUtilsMessengerCreateInfoEXT createInfo = getDebugMessengerCreateInfo();
        if (function(vkInstance, &createInfo, BL_VK_ALLOCATOR, &debugMessenger) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create debug messenger [{0}]", functionName);
            return false;
        }
        return true;
    }

    void Vulkan::destroyDebugMessenger() {
        const char* functionName = "vkDestroyDebugUtilsMessengerEXT";
        auto function = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, functionName);
        if (function == nullptr) {
            BL_LOG_ERROR("Could not look up address of extension function [{0}]", functionName);
            return;
        }
        function(vkInstance, debugMessenger, BL_VK_ALLOCATOR);
    }

    VkDebugUtilsMessengerCreateInfoEXT Vulkan::getDebugMessengerCreateInfo() const {
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

    bool Vulkan::hasValidationLayers(const std::vector<const char*>& validationLayers) const {
        std::vector<VkLayerProperties> availableValidationLayers = findAvailableValidationLayers();
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

    std::vector<VkLayerProperties> Vulkan::findAvailableValidationLayers() const {
        uint32_t count;
        VkLayerProperties* layerProperties = nullptr;
        vkEnumerateInstanceLayerProperties(&count, layerProperties);
        std::vector<VkLayerProperties> layers(count);
        vkEnumerateInstanceLayerProperties(&count, layers.data());
        return layers;
    }

    bool Vulkan::hasExtensions(const std::vector<const char*>& extensions) const {
        std::vector<VkExtensionProperties> availableExtensions = findAvailableExtensions();
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

    std::vector<VkExtensionProperties> Vulkan::findAvailableExtensions() const {
        uint32_t extensionCount = 0;
        const char* layerName = nullptr;
        VkExtensionProperties* extensionProperties = nullptr;
        vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, extensionProperties);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, extensions.data());
        return extensions;
    }
}
