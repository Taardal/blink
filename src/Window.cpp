#include "Window.h"
#include "Log.h"

namespace blink
{
    Window::Window(const Config& config, GraphicsContext* graphicsContext)
            : config(config),
              graphicsContext(graphicsContext),
              glfwWindow(nullptr)
    {
        InitGlfw();
        SetGlfwWindowHints(graphicsContext);
        glfwWindow = CreateGlfwWindow();
        graphicsContext->Init(glfwWindow);
    }

    Window::~Window()
    {
        DestroyGlfwWindow();
        TerminateGlfw();
    }

    void Window::OnUpdate() const
    {
        glfwPollEvents();
        glfwSwapBuffers(glfwWindow);
    }

    void Window::InitGlfw() const
    {
        ST_LOG_DEBUG(ST_TAG, "Initializing GLFW");
        if (glfwInit())
        {
            glfwSetErrorCallback(OnGlfwError);
            ST_LOG_INFO(ST_TAG, "Initialized GLFW");
        }
        else
        {
            ST_LOG_CRITICAL(ST_TAG, "Could not initialize GLFW");
        }
    }

    void Window::OnGlfwError(int32_t error, const char* description)
    {
        ST_LOG_ERROR(ST_TAG_TYPE(Window), "GLFW error [{0}: {1}]", error, description);
    }

    void Window::SetGlfwWindowHints(GraphicsContext* graphicsContext) const
    {
        const GraphicsContext::Config& graphicsConfig = graphicsContext->GetConfig();
        ST_LOG_DEBUG(ST_TAG, "Setting GLFW context version [{0}.{1}]", graphicsConfig.VersionMajor, graphicsConfig.VersionMinor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, graphicsConfig.VersionMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, graphicsConfig.VersionMinor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_MAXIMIZED, this->config.Maximized ? GLFW_TRUE : GLFW_FALSE);
#ifdef ST_PLATFORM_MACOS
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
#endif
    }

    GLFWwindow* Window::CreateGlfwWindow() const
    {
        ST_LOG_DEBUG(ST_TAG, "Creating GLFW window [{0}, {1}x{2}]", config.Title, config.Width, config.Height);
        GLFWmonitor* fullscreenMonitor = nullptr;
        GLFWwindow* sharedWindow = nullptr;
        GLFWwindow* glfwWindow = glfwCreateWindow(config.Width, config.Height, config.Title.c_str(), fullscreenMonitor, sharedWindow);
        if (glfwWindow != nullptr)
        {
            ST_LOG_INFO(ST_TAG, "Created GLFW window");
        }
        else
        {
            ST_LOG_ERROR(ST_TAG, "Could not create GLFW window");
        }
        return glfwWindow;
    }

    void Window::DestroyGlfwWindow() const
    {
        ST_LOG_DEBUG(ST_TAG, "Destroying GLFW window");
        glfwDestroyWindow(glfwWindow);
        ST_LOG_INFO(ST_TAG, "Destroyed GLFW window");
    }

    void Window::TerminateGlfw() const
    {
        ST_LOG_DEBUG(ST_TAG, "Terminating GLFW");
        glfwTerminate();
        ST_LOG_INFO(ST_TAG, "Terminated GLFW");
    }
}
