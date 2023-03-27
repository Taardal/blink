#include "Window.h"
#include "system/Log.h"
#include "GLFW/glfw3.h"

namespace Blink {
    Window::Window()
            : glfwWindow(nullptr) {
    }

    WindowSize Window::getSizeInPixels() const {
        int32_t width = 0;
        int32_t height = 0;
        getSizeInPixels(&width, &height);
        return { width, height };
    }

    void Window::getSizeInPixels(int32_t* width, int32_t* height) const {
        glfwGetFramebufferSize(glfwWindow, width, height);
    };

    void Window::setResizeListener(const std::function<void(uint32_t, uint32_t)>& onResize) {
        callbackData.onResize = onResize;
    };

    void Window::setMinimizeListener(const std::function<void(bool)>& onMinimize) {
        callbackData.onMinimize = onMinimize;
    };

    bool Window::initialize(const AppConfig& config) {
        bool glfwInitialized = glfwInit();
        if (!glfwInitialized) {
            BL_LOG_ERROR("Could not initialize GLFW");
            return false;
        }
        BL_LOG_INFO("Initialized GLFW");

        glfwSetErrorCallback(onGlfwError);

        // Because GLFW was originally designed to create an OpenGL context,
        // we need to tell it to not create an OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwWindowHint(GLFW_RESIZABLE, config.windowResizable);
        glfwWindowHint(GLFW_MAXIMIZED, config.windowMaximized);

        GLFWmonitor* fullscreenMonitor = nullptr;
        GLFWwindow* sharedWindow = nullptr;
        glfwWindow = glfwCreateWindow(
                (int32_t) config.windowWidth,
                (int32_t) config.windowHeight,
                config.windowTitle.c_str(),
                fullscreenMonitor,
                sharedWindow
        );
        if (!glfwWindow) {
            BL_LOG_ERROR("Could not create GLFW window");
            return false;
        }
        BL_LOG_INFO("Created GLFW window");

        glfwSetWindowUserPointer(glfwWindow, &callbackData);
        glfwSetFramebufferSizeCallback(glfwWindow, onFramebufferSizeChange);
        glfwSetWindowIconifyCallback(glfwWindow, onWindowIconifyChange);
        glfwSetKeyCallback(glfwWindow, onKeyChange);

        return true;
    }

    void Window::terminate() const {
        glfwDestroyWindow(glfwWindow);
        BL_LOG_INFO("Destroyed GLFW window");
        glfwTerminate();
        BL_LOG_INFO("Terminated GLFW");
    }

    void Window::onUpdate() const {
        glfwPollEvents();
        //glfwSwapBuffers(glfwWindow);
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(glfwWindow);
    }

    bool Window::isVulkanSupported() const {
        return glfwVulkanSupported() == GLFW_TRUE;
    }

    std::vector<const char*> Window::getRequiredVulkanExtensions() const {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        if (glfwExtensions == nullptr) {
            BL_LOG_ERROR("Could not get required Vulkan extensions");
            return {};
        }
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        return extensions;
    }

    VkResult Window::createVulkanSurface(VkInstance vulkanInstance, VkSurfaceKHR* surface, VkAllocationCallbacks* allocator) const {
        return glfwCreateWindowSurface(vulkanInstance, glfwWindow, allocator, surface);
    }

    void Window::onGlfwError(int32_t error, const char *description) {
        BL_LOG_ERROR("GLFW error [{0}: {1}]", error, description);
    }

    void Window::onKeyChange(GLFWwindow* glfwWindow, int32_t key, int32_t scanCode, int32_t action, int32_t mods) {
        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(glfwWindow, true);
        }
    }

    void Window::waitUntilNotMinimized() const {
        int width = 0;
        int height = 0;
        getSizeInPixels(&width, &height);

        bool iconified = isIconified();

        while (width == 0 || height == 0 || iconified) {
            getSizeInPixels(&width, &height);
            iconified = isIconified();
            glfwWaitEvents();
        }
    }

    bool Window::isIconified() const {
        return glfwGetWindowAttrib(glfwWindow, GLFW_ICONIFIED) == 1;
    }

    void Window::onFramebufferSizeChange(GLFWwindow* glfWwindow, int width, int height) {
        auto userPointer = (CallbackData*) glfwGetWindowUserPointer(glfWwindow);
        userPointer->onResize(width, height);
    }

    void Window::onWindowIconifyChange(GLFWwindow* glfWwindow, int iconified) {
        auto userPointer = (CallbackData*) glfwGetWindowUserPointer(glfWwindow);
        bool minimized = iconified == 1;
        userPointer->onMinimize(minimized);
    }
}
