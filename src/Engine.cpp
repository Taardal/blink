#include "Engine.h"

namespace blink
{
    Engine::Engine(const char* luaFilePath, Config& defaultConfig)
            : fileSystem(nullptr),
              luaClient(nullptr),
              luaEngine(nullptr),
              graphicsContext(nullptr),
              window(nullptr),
              application(nullptr)
    {
        Log::SetLevel(defaultConfig.LogLevel);

        fileSystem = new FileSystem();

        luaClient = new LuaClient();
        luaEngine = new LuaEngine(luaFilePath, luaClient, fileSystem);
        const Config& config = luaEngine->OnConfigure(defaultConfig);

        graphicsContext = new GraphicsContext(config.GraphicsConfig);
        window = new Window(config.WindowConfig, graphicsContext);
        application = new Application(window, luaEngine);
    }

    Engine::~Engine()
    {
        delete application;
        delete window;
        delete graphicsContext;
        delete luaEngine;
        delete luaClient;
        delete fileSystem;
    }

    void Engine::Run() const
    {
        application->Run();
    }
}
