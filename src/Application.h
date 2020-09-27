#pragma once

#include "Window.h"
#include "Renderer.h"
#include "LuaBinding.h"

namespace blink
{
    class Application : LuaBindingListener
    {
    private:
        LuaBinding* luaBinding;
        Window* window;
        GraphicsContext* graphicsContext;
        Renderer* renderer;
        bool running;

    public:
        Application(LuaBinding* luaEngine, Window* window, GraphicsContext* graphicsContext, Renderer* renderer);

        void Run(const char* mainLuaFilePath);

        void Init(Config& config);

    private:
        void OnDrawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) override;
    };
}


