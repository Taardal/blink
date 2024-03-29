#include "Window.h"

#include "KeyEvent.h"
#include "MouseEvent.h"
#include "WindowEvent.h"
#include "system/Log.h"
#include "GLFW/glfw3.h"

namespace Blink {
    Window::Window(const AppConfig& config) {
        bool glfwInitialized = glfwInit();
        if (!glfwInitialized) {
            throw std::runtime_error("Could not initialize GLFW");
        }
        BL_LOG_INFO("Initialized GLFW");

        glfwSetErrorCallback(onGlfwError);

        // Because GLFW was originally designed to create an OpenGL context, and we're using Vulkan,
        // we need to tell it to _not_ create an OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwWindowHint(GLFW_RESIZABLE, config.windowResizable);
        glfwWindowHint(GLFW_MAXIMIZED, config.windowMaximized);

        GLFWmonitor* fullscreenMonitor = nullptr;
        GLFWwindow* sharedWindow = nullptr;
        glfwWindow = glfwCreateWindow(
            config.windowWidth,
            config.windowHeight,
            config.name.c_str(),
            fullscreenMonitor,
            sharedWindow
        );
        if (!glfwWindow) {
            throw std::runtime_error("Could not create GLFW window");
        }
        BL_LOG_INFO("Created GLFW window");

        glfwSetWindowUserPointer(glfwWindow, &userPointer);
        glfwSetKeyCallback(glfwWindow, onKeyChange);
        glfwSetMouseButtonCallback(glfwWindow, onMouseButtonChange);
        glfwSetWindowCloseCallback(glfwWindow, onWindowCloseChange);
        glfwSetWindowIconifyCallback(glfwWindow, onWindowIconifyChange);
        glfwSetFramebufferSizeCallback(glfwWindow, onFramebufferSizeChange);
    }

    Window::~Window() {
        glfwDestroyWindow(glfwWindow);
        BL_LOG_INFO("Destroyed GLFW window");
        glfwTerminate();
        BL_LOG_INFO("Terminated GLFW");
    }

    void Window::setEventListener(const std::function<void(Event&)>& onEvent) {
        userPointer.onEvent = onEvent;
    }

    double Window::update() const {
        glfwPollEvents();
        return glfwGetTime();
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(glfwWindow);
    }

    void Window::setShouldClose(bool shouldClose) const {
        glfwSetWindowShouldClose(glfwWindow, shouldClose);
    }

    bool Window::isKeyPressed(uint16_t key) const {
        return glfwGetKey(glfwWindow, key) == GLFW_PRESS;
    }

    void Window::getSizeInPixels(int32_t* width, int32_t* height) const {
        glfwGetFramebufferSize(glfwWindow, width, height);
    }

    WindowSize Window::getSizeInPixels() const {
        int32_t width = 0;
        int32_t height = 0;
        getSizeInPixels(&width, &height);
        return { width, height };
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

    VkResult Window::createVulkanSurface(
        VkInstance vulkanInstance,
        VkSurfaceKHR* surface,
        VkAllocationCallbacks* allocator
    ) const {
        return glfwCreateWindowSurface(vulkanInstance, glfwWindow, allocator, surface);
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

    void Window::onGlfwError(int32_t error, const char* description) {
        BL_LOG_ERROR("GLFW error [{0}: {1}]", error, description);
    }

    void Window::onKeyChange(GLFWwindow* glfwWindow, int32_t key, int32_t scanCode, int32_t action, int32_t mods) {
        if (action == GLFW_PRESS) {
            KeyPressedEvent event(key, mods, scanCode);
            sendEvent(event, glfwWindow);
        } else if (action == GLFW_RELEASE) {
            KeyReleasedEvent event(key, mods, scanCode);
            sendEvent(event, glfwWindow);
        } else if (action == GLFW_REPEAT) {
            KeyRepeatedEvent event(key, mods, scanCode);
            sendEvent(event, glfwWindow);
        }
    }

    void Window::onMouseButtonChange(GLFWwindow* glfwWindow, int32_t button, int32_t action, int32_t mods) {
        if (action == GLFW_PRESS) {
            MouseButtonPressedEvent event(button);
            sendEvent(event, glfwWindow);
        }
        if (action == GLFW_RELEASE) {
            MouseButtonReleasedEvent event(button);
            sendEvent(event, glfwWindow);
        }
    }

    void Window::onWindowCloseChange(GLFWwindow* glfwWindow) {
        WindowCloseEvent event{};
        sendEvent(event, glfwWindow);
    }

    void Window::onFramebufferSizeChange(GLFWwindow* glfwWindow, int width, int height) {
        WindowResizeEvent event(width, height);
        sendEvent(event, glfwWindow);
    }

    void Window::onWindowIconifyChange(GLFWwindow* glfwWindow, int iconified) {
        bool minimized = iconified == 1;
        WindowMinimizeEvent event(minimized);
        sendEvent(event, glfwWindow);
    }

    void Window::sendEvent(Event& event, GLFWwindow* glfwWindow) {
        BL_LOG_DEBUG(event.toString());
        std::cout << "NOTICE ME" << event << std::endl;
        std::cout << "NOTICE ME" << event << std::endl;
        std::cout << "NOTICE ME" << event << std::endl;
        auto userPointer = (UserPointer*) glfwGetWindowUserPointer(glfwWindow);
        userPointer->onEvent(event);
    }
}
