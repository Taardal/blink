#include "Engine.h"

namespace blink
{
    Engine::Engine()
            : fileSystem(new FileSystem()),
              luaClient(new LuaClient()),
              luaGraphicsEngine(new LuaGraphicsBinding(luaClient)),
              luaEngine(new LuaBinding(luaGraphicsEngine, luaClient, fileSystem)),
              window(new Window()),
              graphicsContext(new GraphicsContext()),
              renderer(new Renderer()),
              application(new Application(luaEngine, window, graphicsContext, renderer))
    {
    }

    Engine::~Engine()
    {
        delete application;
        delete renderer;
        delete window;
        delete graphicsContext;
        delete luaEngine;
        delete luaClient;
        delete fileSystem;
    }

    void Engine::Init(Config& config) const
    {
        application->Init(config);
    }

    void Engine::Run(const char* mainLuaFilePath) const
    {
        luaEngine->Run(mainLuaFilePath);
        application->Run();
    }
}
