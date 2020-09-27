#include "Application.h"

namespace blink
{
    Application::Application(LuaBinding* luaEngine, Window* window, GraphicsContext* graphicsContext, Renderer* renderer)
            : luaBinding(luaEngine),
              window(window),
              graphicsContext(graphicsContext),
              renderer(renderer),
              running(false)
    {
    }

    void Application::Init(Config& config)
    {
        Log::SetLevel(config.LogLevel);
        luaBinding->Init(this);
        luaBinding->OnConfigure(config);
        window->Init(config.WindowConfig, config.GraphicsConfig);
        graphicsContext->Init(config.GraphicsConfig);
    }

    void Application::Run(const char* mainLuaFilePath)
    {
        ST_LOG_INFO(ST_TAG, "Running application...");
        luaBinding->Run(mainLuaFilePath);
        luaBinding->OnCreate();
        glClearColor(0.8f, 0.2f, 0.3f, 0.2f);

        running = false;
        while (running)
        {
        }
        glClear(GL_COLOR_BUFFER_BIT);
        luaBinding->OnUpdate();
        luaBinding->OnDraw();
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
