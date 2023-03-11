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
        glfwGetFramebufferSize(glfwWindow, &width, &height);
        return { width, height };
    }

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

        //glfwMakeContextCurrent(glfwWindow);

        //glfwSetWindowUserPointer(glfwWindow, &userPointer);
        //glfwSetFramebufferSizeCallback(glfwWindow, onFramebufferSizeChange);
        //glfwSetWindowIconifyCallback(glfwWindow, onWindowIconifyChange);
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

    void Window::onGlfwError(int32_t error, const char *description) {
        BL_LOG_ERROR("GLFW error [{0}: {1}]", error, description);
    }

    void Window::onKeyChange(GLFWwindow* glfwWindow, int32_t key, int32_t scanCode, int32_t action, int32_t mods) {
        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(glfwWindow, true);
        }
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
}
