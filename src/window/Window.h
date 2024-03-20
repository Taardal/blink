#pragma once

#include "AppConfig.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace Blink {
    struct WindowSize {
        int32_t width;
        int32_t height;
    };

    class Window {
    private:
        struct CallbackData {
            std::function<void(uint32_t, uint32_t)> onResize;
            std::function<void(bool)> onMinimize;
        };

    private:
        GLFWwindow* glfwWindow = nullptr;
        CallbackData callbackData;
        double lastFrameTime = 0.0;

    public:
        explicit Window(const AppConfig& config);

        ~Window();

        GLFWwindow* getGlfwWindow() const;

        WindowSize getSizeInPixels() const;

        void getSizeInPixels(int32_t* width, int32_t* height) const;

        void setResizeListener(const std::function<void(uint32_t, uint32_t)>& onResize);

        void setMinimizeListener(const std::function<void(bool)>& onMinimize);

        double update();

        bool shouldClose() const;

        void waitUntilNotMinimized() const;

        bool isVulkanSupported() const;

        bool isKeyPressed(uint16_t key) const;

        std::vector<const char*> getRequiredVulkanExtensions() const;

        VkResult createVulkanSurface(VkInstance vulkanInstance, VkSurfaceKHR* surface, VkAllocationCallbacks* allocator = nullptr) const;

    private:
        bool isIconified() const;

        static void onGlfwError(int32_t error, const char* description);

        static void onKeyChange(GLFWwindow* glfwWindow, int32_t key, int32_t scanCode, int32_t action, int32_t mods);

        static void onFramebufferSizeChange(GLFWwindow* glfWwindow, int width, int height);

        static void onWindowIconifyChange(GLFWwindow* glfWwindow, int iconified);
    };
}