#pragma once

#include "FileSystem.h"
#include "LuaClient.h"
#include "LuaGraphicsEngine.h"
#include "LuaEngine.h"
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
        LuaGraphicsEngine* luaGraphicsEngine;
        LuaEngine* luaEngine;
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


