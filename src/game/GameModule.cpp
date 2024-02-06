#include "GameModule.h"

#include <utility>

namespace Blink {
    GameModule::GameModule(
        const AppConfig& appConfig,
        WindowModule* windowModule,
        GraphicsModule* graphicsModule
    ) : appConfig(appConfig),
        camera(new Camera(windowModule->getWindow())),
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
        if (!camera->initialize()) {
            BL_LOG_ERROR("Could not initialize camera");
            return false;
        }
        return true;
    }

    void GameModule::terminate() {
    }
}
