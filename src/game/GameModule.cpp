#include "GameModule.h"

#include <utility>

namespace Blink {
    GameModule::GameModule(
        const AppConfig& appConfig,
        WindowModule* windowModule,
        GraphicsModule* graphicsModule
    ) : appConfig(appConfig),
        keyboard(new Keyboard(windowModule->getWindow())),
        camera(new Camera(windowModule->getWindow(), keyboard)),
        scene(new Scene(keyboard)),
        game(new Game(scene, camera, windowModule->getWindow(), graphicsModule->getRenderer())) {
    }

    GameModule::~GameModule() {
        delete game;
        delete scene;
        delete camera;
        delete keyboard;
    }

    Game* GameModule::getGame() const {
        return game;
    }

    bool GameModule::initialize(const AppConfig& appConfig) const {
        if (!keyboard->initialize()) {
            BL_LOG_ERROR("Could not initialize keyboard");
            return false;
        }
        if (!camera->initialize()) {
            BL_LOG_ERROR("Could not initialize camera");
            return false;
        }
        if (!scene->initialize()) {
            BL_LOG_ERROR("Could not initialize scene");
            return false;
        }
        if (!game->initialize()) {
            BL_LOG_ERROR("Could not initialize game");
            return false;
        }
        return true;
    }

    void GameModule::terminate() const {
        game->terminate();
        scene->terminate();
        camera->terminate();
        keyboard->terminate();
    }
}
