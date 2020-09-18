#include "Application.h"

namespace blink
{
    Application::Application(LuaEngine* luaEngine, Window* window, GraphicsContext* graphicsContext, Renderer* renderer)
            : luaEngine(luaEngine),
              window(window),
              graphicsContext(graphicsContext),
              renderer(renderer),
              running(false)
    {
    }

    void Application::Init(Config& defaultConfig)
    {
        Log::SetLevel(defaultConfig.LogLevel);
        luaEngine->Init(this);
        const Config& config = luaEngine->OnConfigure(defaultConfig);
        window->Init(config.WindowConfig, config.GraphicsConfig);
        graphicsContext->Init(config.GraphicsConfig);
    }

    void Application::Run()
    {
        ST_LOG_INFO(ST_TAG, "Running application...");
        //running = true;

        luaEngine->OnCreate();
        glClearColor(0.8f, 0.2f, 0.3f, 0.2f);

        while (running)
        {
        }
        glClear(GL_COLOR_BUFFER_BIT);
        luaEngine->OnUpdate();
        luaEngine->OnDraw();
        window->OnUpdate();
    }

    void Application::OnDrawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        ST_LOG_TRACE(ST_TAG, "Application :: OnDrawRectangle");
        Renderer::Quad quad{};
        quad.Position = position;
        quad.Size = size;
        quad.Color = color;
        renderer->Submit(quad);
    }
}
