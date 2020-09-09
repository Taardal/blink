#include "GraphicsContext.h"
#include "Log.h"

namespace blink
{
    GraphicsContext::GraphicsContext(const GraphicsContext::Config& config)
            : config(config)
    {}

    void GraphicsContext::Init(GLFWwindow* glfwWindow) const
    {
        glfwMakeContextCurrent(glfwWindow);
        InitGlad();
        LogContext();
    }

    const GraphicsContext::Config& GraphicsContext::GetConfig() const
    {
        return config;
    }

    void GraphicsContext::InitGlad() const
    {
        bool initialized = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress) != 0;
#if 1
        if (initialized)
        {
            ST_LOG_INFO(ST_TAG, "Initialized GLAD");
        }
        else
        {
            ST_LOG_CRITICAL(ST_TAG, "Could not initialize GLAD");
        }
#endif
    }

    void GraphicsContext::LogContext() const
    {
#if 1
        ST_LOG_INFO(ST_TAG, "Vendor [{0}]", glGetString(GL_VENDOR));
        ST_LOG_INFO(ST_TAG, "Renderer [{0}]", glGetString(GL_RENDERER));
        ST_LOG_INFO(ST_TAG, "Version [{0}]", glGetString(GL_VERSION));
#endif
    }
}


