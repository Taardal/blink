#pragma once

#include "Camera.h"
#include "Scene.h"
#include "Game.h"
#include "window/WindowModule.h"
#include "graphics/GraphicsModule.h"
#include "lua/LuaModule.h"


namespace Blink {
    struct GameModule {
        Camera* camera;
        Scene* scene;
        Game* game;

        GameModule(
            WindowModule* windowModule,
            GraphicsModule* graphicsModule,
            LuaModule* luaModule
        );

        ~GameModule();

        bool initialize() const;

        void terminate() const;
    };
}
