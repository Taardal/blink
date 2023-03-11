#pragma once

#include "AppConfig.h"
#include <string>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace Blink
{
    struct WindowSize {
        int32_t width;
        int32_t height;
    };

    class Window
    {
    private:
        GLFWwindow* glfwWindow = nullptr;

    public:
        Window();

        WindowSize getSizeInPixels() const;

        bool initialize(const AppConfig& config);

        void terminate() const;

        void onUpdate() const;

        bool shouldClose() const;

        bool isVulkanSupported() const;

        std::vector<const char*> getRequiredVulkanExtensions() const;

        VkResult createVulkanSurface(VkInstance vulkanInstance, VkSurfaceKHR* surface, VkAllocationCallbacks* allocator = nullptr) const;

    private:
        static void onGlfwError(int32_t error, const char* description);

        static void onKeyChange(GLFWwindow* glfwWindow, int32_t key, int32_t scanCode, int32_t action, int32_t mods);
    };
}