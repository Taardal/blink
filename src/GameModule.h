#pragma once

#include "graphics/GraphicsModule.h"
#include "window/WindowModule.h"
#include "system/FileSystem.h"
#include "window/Window.h"
#include "Game.h"


namespace Blink {
    class GameModule {
    private:
        Game* game;

    public:
        GameModule(WindowModule* windowModule, GraphicsModule* graphicsModule);

        ~GameModule();

        Game* getGame() const;

        bool initialize(const Config& config);

        void terminate();
    };
}
