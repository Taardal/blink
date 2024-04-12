#pragma once

#include "Event.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Blink {
    struct WindowSize {
        int32_t width;
        int32_t height;
    };

    struct WindowConfig {
        std::string title;
        int32_t width;
        int32_t height;
        bool maximized;
        bool resizable;
        std::function<void(Event&)> onEvent;
    };

    class Window {
    private:
        struct UserPointer {
            std::function<void(Event&)> onEvent;
        };

    private:
        GLFWwindow* glfwWindow = nullptr;
        UserPointer userPointer;

    public:
        explicit Window(const WindowConfig& config);

        ~Window();

        double update() const;

        bool shouldClose() const;

        void setShouldClose(bool shouldClose) const;

        bool isKeyPressed(uint16_t key) const;

        WindowSize getSizeInPixels() const;

        void getSizeInPixels(int32_t* width, int32_t* height) const;

        bool isVulkanSupported() const;

        std::vector<const char*> getRequiredVulkanExtensions() const;

        VkResult createVulkanSurface(VkInstance vulkanInstance, VkSurfaceKHR* surface, VkAllocationCallbacks* allocator = nullptr) const;

        void waitUntilNotMinimized() const;

    private:
        bool isIconified() const;

        static void onGlfwError(int32_t error, const char* description);

        static void onKeyChange(GLFWwindow* glfwWindow, int32_t key, int32_t scanCode, int32_t action, int32_t mods);

        static void onMouseButtonChange(GLFWwindow* glfwWindow, int32_t button, int32_t action, int32_t mods);

        static void onWindowCloseChange(GLFWwindow* glfwWindow);

        static void onWindowIconifyChange(GLFWwindow* glfwWindow, int iconified);

        static void onFramebufferSizeChange(GLFWwindow* glfwWindow, int width, int height);

        static void sendEvent(Event& event, GLFWwindow* glfwWindow);
    };
}