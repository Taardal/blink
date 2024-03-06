#pragma once

#include "AppConfig.h"
#include "system/SystemModule.h"
#include "window/WindowModule.h"
#include "graphics/GraphicsModule.h"
#include "lua/LuaModule.h"
#include "game/GameModule.h"

namespace Blink {
    class App {
    private:
        AppConfig appConfig;
        SystemModule* systemModule;
        WindowModule* windowModule;
        GraphicsModule* graphicsModule;
        LuaModule* luaModule;
        GameModule* gameModule;

    public:
        explicit App(AppConfig appConfig);

        ~App();

        void run() const;

    private:
        bool initialize() const;

        void terminate() const;
    };
}


