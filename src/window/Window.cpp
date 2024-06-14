#include "Window.h"

#include "KeyEvent.h"
#include "MouseEvent.h"
#include "WindowEvent.h"
#include "system/Log.h"
#include "GLFW/glfw3.h"

namespace Blink {
    Window::Window(const WindowConfig& config) {
        BL_ASSERT_THROW(glfwInit());
        BL_LOG_INFO("Initialized GLFW");

        glfwSetErrorCallback(onGlfwError);

        // GLFW was originally designed to create an OpenGL context.
        // Because we are using Vulkan, we need to explicitly tell GLFW to _NOT_ create an OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwWindowHint(GLFW_RESIZABLE, config.resizable);
        glfwWindowHint(GLFW_MAXIMIZED, config.maximized);

        GLFWmonitor* fullscreenMonitor = nullptr;
        GLFWwindow* sharedWindow = nullptr;
        glfwWindow = glfwCreateWindow(
            config.width,
            config.height,
            config.title.c_str(),
            fullscreenMonitor,
            sharedWindow
        );
        BL_ASSERT_THROW(glfwWindow != nullptr);
        BL_LOG_INFO("Created GLFW window");

        userPointer.onEvent = config.onEvent;

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

    void Window::setTitle(const char* title) const {
        glfwSetWindowTitle(glfwWindow, title);
    }

    void Window::setTitle(const std::string& title) const {
        setTitle(title.c_str());
    }

    double Window::update() const {
        glfwPollEvents();
        return glfwGetTime();
    }

    double Window::getTime() const {
        return glfwGetTime(); // Seconds
    }

    void Window::pollEvents() const {
        glfwPollEvents();
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

    bool Window::isMouseButtonPressed(uint16_t key) const {
        return glfwGetMouseButton(glfwWindow, key) == GLFW_PRESS;
    }

    glm::vec2 Window::getMousePosition() const {
        double x, y;
        glfwGetCursorPos(glfwWindow, &x, &y);
        return {x, y};
    }

    void Window::setMouseCursorHidden(bool hidden) const {
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, hidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    WindowSize Window::getSizeInPixels() const {
        int32_t width = 0;
        int32_t height = 0;
        getSizeInPixels(&width, &height);
        return { width, height };
    }

    void Window::getSizeInPixels(int32_t* width, int32_t* height) const {
        glfwGetFramebufferSize(glfwWindow, width, height);
    }

    float Window::getAspectRatio() const {
        WindowSize size = getSizeInPixels();
        return (float) size.width / (float) size.height;
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
        BL_LOG_TRACE(event.toString());
        auto userPointer = (UserPointer*) glfwGetWindowUserPointer(glfwWindow);
        userPointer->onEvent(event);
    }
}
