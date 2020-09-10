#pragma once
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
        LuaClient* lua;
        FileSystem* fileSystem;

    public:
        LuaEngine(const char* mainFilePath, LuaClient* lua, FileSystem* fileSystem);

        [[nodiscard]] Config OnConfigure(Config& defaultConfig);

        void OnCreate();

        void OnUpdate();

        void OnDraw();

    private:
        Config& LoadLuaConfig(Config& config);
    };
}


