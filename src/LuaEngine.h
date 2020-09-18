#pragma once

#include "LuaGraphicsEngine.h"
#include "LuaClient.h"
#include "FileSystem.h"
#include "Config.h"

namespace blink
{
    class LuaEngine
    {
    private:
        static const char* LUA_PACKAGE_PATH;
        static const char* LUA_MAIN_PATH;
        static const char* LUA_CONFIG_PATH;
        LuaGraphicsEngine* graphicsEngine;
        LuaClient* lua;
        FileSystem* fileSystem;
        LuaEngineListener* listener;

    public:
        LuaEngine(LuaGraphicsEngine* graphicsEngine, LuaClient* lua, FileSystem* fileSystem);

        void Init(LuaEngineListener* listener);

        void Run(const char* mainFilePath);

        [[nodiscard]] Config& OnConfigure(Config& defaultConfig);

        void OnCreate();

        void OnUpdate();

        void OnDraw();

    private:
        Config& LoadLuaConfig(Config& config);
    };
}


