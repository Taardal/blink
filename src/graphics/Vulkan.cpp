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

    Vulkan::Vulkan(const VulkanConfig& vulkanConfig, Window* window) : window(window) {
        this->validationLayersEnabled = vulkanConfig.validationLayersEnabled;
        if (!window->isVulkanSupported()) {
            throw std::runtime_error("Vulkan is not supported");
        }
        std::vector<const char*> requiredExtensions = window->getRequiredVulkanExtensions();
        if (std::count(requiredExtensions.begin(), requiredExtensions.end(), "VK_KHR_surface") == 0) {
            throw std::runtime_error("Vulkan surface extension is not supported");
        }
        if (Environment::isMacOS()) {
            requiredExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        }
        if (validationLayersEnabled) {
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        if (!hasExtensions(requiredExtensions)) {
            throw std::runtime_error("Could not find required extensions");
        }
        std::vector<const char*> validationLayers;
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        if (validationLayersEnabled) {
            validationLayers.push_back("VK_LAYER_KHRONOS_validation");
            if (!hasValidationLayers(validationLayers)) {
                throw std::runtime_error("Could not find validation layers");
            }
            debugMessengerCreateInfo = getDebugMessengerCreateInfo();
        }
        if (!createInstance(vulkanConfig, requiredExtensions, validationLayers, debugMessengerCreateInfo)) {
            throw std::runtime_error("Could not create Vulkan instance");
        }
        BL_LOG_INFO("Created Vulkan instance");
        if (validationLayersEnabled) {
            if (!createDebugMessenger(debugMessengerCreateInfo)) {
                throw std::runtime_error("Could not create debug messenger");
            }
        }
        if (!createSurface()) {
            throw std::runtime_error("Could not create surface");
        }
    }

    Vulkan::~Vulkan() {
        destroySurface();
        if (validationLayersEnabled) {
            destroyDebugMessenger();
        }
        destroyInstance();
        BL_LOG_INFO("Destroyed Vulkan instance");
    }

    std::vector<VkPhysicalDevice> Vulkan::getPhysicalDevices() const {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, devices.data());
        return devices;
    }

    VkSurfaceKHR Vulkan::getSurface() const {
        return surface;
    }

    bool Vulkan::createInstance(
            const VulkanConfig& vulkanConfig,
            const std::vector<const char*>& requiredExtensions,
            const std::vector<const char*>& validationLayers,
            const VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo
    ) {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = vulkanConfig.applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = vulkanConfig.engineName.c_str();
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
        if (validationLayersEnabled) {
            createInfo.pNext = &debugMessengerCreateInfo;
            createInfo.enabledLayerCount = (uint32_t) validationLayers.size();
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }

        return vkCreateInstance(&createInfo, BL_VK_ALLOCATOR, &vulkanInstance) == VK_SUCCESS;
    }

    void Vulkan::destroyInstance() {
        vkDestroyInstance(vulkanInstance, BL_VK_ALLOCATOR);
    }

    bool Vulkan::createDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo) {
        const char* functionName = "vkCreateDebugUtilsMessengerEXT";
        auto function = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vulkanInstance, functionName);
        if (function == nullptr) {
            BL_LOG_ERROR("Could not look up address of extension function [{0}]", functionName);
            return false;
        }
        return function(vulkanInstance, &debugMessengerCreateInfo, BL_VK_ALLOCATOR, &debugMessenger) == VK_SUCCESS;
    }

    void Vulkan::destroyDebugMessenger() {
        const char* functionName = "vkDestroyDebugUtilsMessengerEXT";
        auto function = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vulkanInstance, functionName);
        if (function == nullptr) {
            BL_LOG_ERROR("Could not look up address of extension function [{0}]", functionName);
            return;
        }
        function(vulkanInstance, debugMessenger, BL_VK_ALLOCATOR);
    }

    bool Vulkan::createSurface() {
        return window->createVulkanSurface(vulkanInstance, &surface, BL_VK_ALLOCATOR) == VK_SUCCESS;
    }

    void Vulkan::destroySurface() const {
        vkDestroySurfaceKHR(vulkanInstance, surface, BL_VK_ALLOCATOR);
    }

    bool Vulkan::hasValidationLayers(const std::vector<const char*>& validationLayers) const {
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

    std::vector<VkLayerProperties> Vulkan::getAvailableValidationLayers() const {
        uint32_t count;
        VkLayerProperties* layerProperties = nullptr;
        vkEnumerateInstanceLayerProperties(&count, layerProperties);
        std::vector<VkLayerProperties> layers(count);
        vkEnumerateInstanceLayerProperties(&count, layers.data());
        return layers;
    }

    bool Vulkan::hasExtensions(const std::vector<const char*>& extensions) const {
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

    std::vector<VkExtensionProperties> Vulkan::getAvailableExtensions() const {
        uint32_t extensionCount = 0;
        const char* layerName = nullptr;
        VkExtensionProperties* extensionProperties = nullptr;
        vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, extensionProperties);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, extensions.data());
        return extensions;
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
}
