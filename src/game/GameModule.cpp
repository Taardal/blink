#include "pch.h"
#include "GameModule.h"

namespace Blink {
    GameModule::GameModule(
        WindowModule* windowModule,
        GraphicsModule* graphicsModule
    ) : camera(new Camera(windowModule->window, windowModule->keyboard)),
        scene(new Scene(windowModule->keyboard)),
        game(new Game(scene, camera, windowModule->window, graphicsModule->renderer)) {
    }

    GameModule::~GameModule() {
        delete game;
        delete scene;
        delete camera;
    }

    bool GameModule::initialize() const {
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
    }
}
