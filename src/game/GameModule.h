#pragma once

#include "Camera.h"
#include "Game.h"
#include "window/WindowModule.h"
#include "graphics/GraphicsModule.h"


namespace Blink {
    class GameModule {
    private:
        AppConfig appConfig;
        Camera* camera;
        Game* game;

    public:
        GameModule(const AppConfig& appConfig, WindowModule* windowModule, GraphicsModule* graphicsModule);

        ~GameModule();

        Game* getGame() const;

        bool initialize(const AppConfig& appConfig);

        void terminate();
    };
}
