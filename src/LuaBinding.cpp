#include "LuaBinding.h"

namespace blink
{
    const char* LuaBinding::LUA_PACKAGE_PATH = "res/?.lua";
    const char* LuaBinding::LUA_CONFIG_PATH = "res/config.lua";
    const char* LuaBinding::LUA_MAIN_PATH = "res/main.lua";

    LuaBinding::LuaBinding(LuaGraphicsBinding* graphicsBinding, LuaClient* lua, FileSystem* fileSystem)
            : graphicsBinding(graphicsBinding),
              lua(lua),
              fileSystem(fileSystem),
              listener(nullptr)
    {
    }

    void LuaBinding::Init(LuaBindingListener* listener)
    {
        lua->EnableStandardLibraries();
        lua->Table().SetGlobal("blink");
        graphicsBinding->Init(listener);
    }

    void LuaBinding::Run(const char* mainFilePath)
    {
        lua->AddPackagePath(LUA_PACKAGE_PATH);
        lua->RunFile(LUA_CONFIG_PATH);
        lua->RunFile(mainFilePath ? mainFilePath : LUA_MAIN_PATH);
    }

    void LuaBinding::OnConfigure(Config& config)
    {
        lua->Clear();
        lua->Global("blink");

        lua->Table();
        lua->String(config.WindowConfig.Title.c_str()).SetField("title");
        lua->Number<int>(config.WindowConfig.Width).SetField("width");
        lua->Number<int>(config.WindowConfig.Height).SetField("height");
        lua->SetField("config");

        lua->Field("onConfigure");
        lua->Field("config");
        lua->PrintStack("begin");
        lua->CallFunction(LuaFunctionConfig().WithParameterCount(1));
        lua->PrintStack("end");

        lua->Field("config");
        config.WindowConfig.Title = lua->Field("title").ToString();
        config.WindowConfig.Width = lua->Field("width").ToNumber<int>();
        config.WindowConfig.Height = lua->Field("height").ToNumber<int>();
    }

    void LuaBinding::OnCreate()
    {
        lua->Global("blink").Field("onCreate").CallFunction();
    }

    void LuaBinding::OnUpdate()
    {
        lua->Global("blink").Field("onUpdate").CallFunction();
    }

    void LuaBinding::OnDraw()
    {
        lua->Global("blink").Field("onDraw").CallFunction();
    }

}