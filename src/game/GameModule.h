#pragma once

#include "Camera.h"
#include "Scene.h"
#include "Game.h"
#include "window/WindowModule.h"
#include "graphics/GraphicsModule.h"


namespace Blink {
    struct GameModule {
        Camera* camera;
        Scene* scene;
        Game* game;

        GameModule(WindowModule* windowModule, GraphicsModule* graphicsModule);

        ~GameModule();

        bool initialize() const;

        void terminate() const;
    };
}
