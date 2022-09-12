#include "GameModule.h"

namespace Blink {
    GameModule::GameModule(WindowModule* windowModule, GraphicsModule* graphicsModule)
            : game(new Game(windowModule->getWindow(), graphicsModule->getRenderer())) {
    }

    GameModule::~GameModule() {
        delete game;
    }

    Game* GameModule::getGame() const {
        return game;
    }

    bool GameModule::initialize(const Config& config) {
        return true;
    }

    void GameModule::terminate() {
    }
}