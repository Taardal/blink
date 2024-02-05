#pragma once

#include "LuaGraphicsEngine.h"
#include "LuaClient.h"
#include "system/FileSystem.h"
#include "AppConfig.h"

namespace Blink
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

        [[nodiscard]] AppConfig& OnConfigure(AppConfig& defaultConfig);

        void OnCreate();

        void OnUpdate();

        void OnDraw();

    private:
        AppConfig& LoadLuaConfig(AppConfig& config);
    };
}


