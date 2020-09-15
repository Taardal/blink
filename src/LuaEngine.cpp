#include "LuaEngine.h"

namespace blink
{
    const char* LuaEngine::LUA_PACKAGE_PATH = "res/?.lua";
    const char* LuaEngine::LUA_CONFIG_PATH = "res/config.lua";
    const char* LuaEngine::LUA_MAIN_PATH = "res/main.lua";

    LuaEngine::LuaEngine(LuaGraphicsEngine* graphicsEngine, LuaClient* lua, FileSystem* fileSystem)
            : graphicsEngine(graphicsEngine),
              lua(lua),
              fileSystem(fileSystem),
              listener(nullptr)
    {
    }

    void LuaEngine::Init(LuaEngineListener* listener)
    {
        lua->EnableStandardLibraries();
        lua->AddPackagePath(LUA_PACKAGE_PATH);
        lua->Table()->SetGlobal("blink");
        graphicsEngine->Init(listener);
    }

    void LuaEngine::Run(const char* mainFilePath)
    {
        lua->RunFile(mainFilePath ? mainFilePath : LUA_MAIN_PATH);
    }

    Config& LuaEngine::OnConfigure(Config& defaultConfig)
    {
        if (fileSystem->Exists(LUA_CONFIG_PATH))
        {
            lua->RunFile(LUA_CONFIG_PATH);
            return LoadLuaConfig(defaultConfig);
        }
        return defaultConfig;
    }

    void LuaEngine::OnCreate()
    {
        lua->Global("blink").Field("onCreate").CallFunction();
        lua->End();
    }

    void LuaEngine::OnUpdate()
    {
        lua->Global("blink").Field("onUpdate").CallFunction();
        lua->End();
    }

    void LuaEngine::OnDraw()
    {
        lua->Global("blink").Field("onDraw").CallFunction();
        lua->End();
    }

    Config& LuaEngine::LoadLuaConfig(Config& config)
    {
        lua->Begin();
        lua->Global("blink");

        lua->Table();

        lua->Table();
        lua->String(config.WindowConfig.Title.c_str()).WriteToField("title");
        lua->Number<int>(config.WindowConfig.Width).WriteToField("width");
        lua->Number<int>(config.WindowConfig.Height).WriteToField("height");
        lua->WriteToField("window");

        lua->Table();
        lua->String(config.GraphicsConfig.GlslVersion.c_str()).WriteToField("glslVersion");
        lua->Number<int>(config.GraphicsConfig.VersionMajor).WriteToField("versionMajor");
        lua->Number<int>(config.GraphicsConfig.VersionMinor).WriteToField("versionMinor");
        lua->WriteToField("graphics");

        lua->WriteToField("config");

        lua->Field("onConfigure");
        lua->Field("config");
        lua->CallFunction(LuaFunctionConfig().WithParameterCount(1));

        lua->Field("config");

        lua->Field("window");
        config.WindowConfig.Title = lua->Field("title").ToString();
        lua->Pop();
        config.WindowConfig.Width = lua->Field("width").ToNumber<int>();
        lua->Pop();
        config.WindowConfig.Height = lua->Field("height").ToNumber<int>();
        lua->Pop(2);

        lua->Field("graphics");
        config.GraphicsConfig.GlslVersion = lua->Field("glslVersion").ToString();
        lua->Pop();
        config.GraphicsConfig.VersionMajor = lua->Field("versionMajor").ToNumber<int>();
        lua->Pop();
        config.GraphicsConfig.VersionMinor = lua->Field("versionMinor").ToNumber<int>();
        lua->Pop(2);

        lua->End();
        return config;
    }
}