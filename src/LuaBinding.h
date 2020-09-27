#pragma once

#include "LuaGraphicsBinding.h"
#include "LuaClient.h"
#include "FileSystem.h"
#include "Config.h"

namespace blink
{
    class LuaBinding
    {
    private:
        static const char* LUA_PACKAGE_PATH;
        static const char* LUA_MAIN_PATH;
        static const char* LUA_CONFIG_PATH;
        LuaGraphicsBinding* graphicsBinding;
        LuaClient* lua;
        FileSystem* fileSystem;
        LuaBindingListener* listener;

    public:
        LuaBinding(LuaGraphicsBinding* graphicsBinding, LuaClient* lua, FileSystem* fileSystem);

        void Init(LuaBindingListener* listener);

        void Run(const char* mainFilePath);

        void OnConfigure(Config& config);

        void OnCreate();

        void OnUpdate();

        void OnDraw();
    };
}


