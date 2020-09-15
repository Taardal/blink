#pragma once

#include "Window.h"
#include "Renderer.h"
#include "LuaEngine.h"

namespace blink
{
    class Application : LuaEngineListener
    {
    private:
        LuaEngine* luaEngine;
        Window* window;
        GraphicsContext* graphicsContext;
        Renderer* renderer;
        bool running;

    public:
        Application(LuaEngine* luaEngine, Window* window, GraphicsContext* graphicsContext, Renderer* renderer);

        void Run();

        void Init(Config& defaultConfig);

    private:
        void OnDrawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) override;
    };
}


