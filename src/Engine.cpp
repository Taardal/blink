#include "Engine.h"

namespace blink
{
    Engine::Engine()
            : fileSystem(new FileSystem()),
              luaClient(new LuaClient()),
              luaGraphicsBinding(new LuaGraphicsBinding(luaClient)),
              luaBinding(new LuaBinding(luaGraphicsBinding, luaClient, fileSystem)),
              window(new Window()),
              graphicsContext(new GraphicsContext()),
              renderer(new Renderer()),
              application(new Application(luaBinding, window, graphicsContext, renderer))
    {
    }

    Engine::~Engine()
    {
        delete application;
        delete renderer;
        delete graphicsContext;
        delete window;
        delete luaBinding;
        delete luaGraphicsBinding;
        delete luaClient;
        delete fileSystem;
    }

    void Engine::Init(Config& config) const
    {
        application->Init(config);
    }

    void Engine::Run(const char* mainLuaFilePath) const
    {
        application->Run(mainLuaFilePath);
    }
}
