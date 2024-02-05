#include "GameModule.h"

#include <utility>

namespace Blink {
    GameModule::GameModule(
        const AppConfig& appConfig,
        WindowModule* windowModule,
        GraphicsModule* graphicsModule
    ) : appConfig(appConfig),
        camera(new Camera(appConfig, windowModule->getWindow())),
        game(new Game(camera, windowModule->getWindow(), graphicsModule->getRenderer())) {
    }

    GameModule::~GameModule() {
        delete game;
        delete camera;
    }

    Game* GameModule::getGame() const {
        return game;
    }

    bool GameModule::initialize(const AppConfig& appConfig) {
        return true;
    }

    void GameModule::terminate() {
    }
}
