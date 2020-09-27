#pragma once

#include "FileSystem.h"
#include "LuaClient.h"
#include "LuaGraphicsBinding.h"
#include "LuaBinding.h"
#include "Window.h"
#include "GraphicsContext.h"
#include "Renderer.h"
#include "Application.h"
#include "Config.h"

namespace blink
{
    class Engine
    {
    private:
        FileSystem* fileSystem;
        LuaClient* luaClient;
        LuaGraphicsBinding* luaGraphicsBinding;
        LuaBinding* luaBinding;
        Window* window;
        GraphicsContext* graphicsContext;
        Renderer* renderer;
        Application* application;

    public:
        Engine();

        virtual ~Engine();

        void Init(Config& config) const;

        void Run(const char* mainLuaFilePath) const;
    };
}


