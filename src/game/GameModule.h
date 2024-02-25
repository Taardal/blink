#pragma once

#include "Camera.h"
#include "Scene.h"
#include "Game.h"
#include "window/WindowModule.h"
#include "graphics/GraphicsModule.h"


namespace Blink {
    class GameModule {
    private:
        AppConfig appConfig;
        Keyboard* keyboard;
        Camera* camera;
        Scene* scene;
        Game* game;

    public:
        GameModule(const AppConfig& appConfig, WindowModule* windowModule, GraphicsModule* graphicsModule);

        ~GameModule();

        Game* getGame() const;

        bool initialize(const AppConfig& appConfig) const;

        void terminate() const;
    };
}
