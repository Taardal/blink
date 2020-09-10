#pragma once

#include "Log.h"
#include "Config.h"
#include "FileSystem.h"
#include "LuaClient.h"
#include "LuaEngine.h"
#include "Window.h"
#include "Application.h"

namespace blink
{
    class Engine
    {
    private:
        FileSystem* fileSystem;
        LuaClient* luaClient;
        LuaEngine* luaEngine;
        GraphicsContext* graphicsContext;
        Window* window;
        Application* application;

    public:
        Engine(const char* luaFilePath, Config& config);

        virtual ~Engine();

        void Run() const;
    };
}


