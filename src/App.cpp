#include "App.h"

#include <utility>

namespace Blink {
    App::App(AppConfig appConfig)
            : appConfig(std::move(appConfig)),
              systemModule(new SystemModule()),
              windowModule(new WindowModule()),
              graphicsModule(new GraphicsModule(systemModule, windowModule)),
              gameModule(new GameModule(appConfig, windowModule, graphicsModule)) {
    }

    App::~App() {
        delete gameModule;
        delete graphicsModule;
        delete windowModule;
        delete systemModule;
    }

    void App::run() const {
        if (!initialize()) {
            BL_LOG_CRITICAL("Could not initialize app");
            return;
        }
        Game* game = gameModule->getGame();
        try {
            game->run();
        } catch (std::exception& e) {
            BL_LOG_CRITICAL("Could not run game: {}", e.what());
        }
        terminate();
    }

    bool App::initialize() const {
        if (!systemModule->initialize(appConfig)) {
            BL_LOG_ERROR("Could not initialize system module");
            return false;
        }
        if (!windowModule->initialize(appConfig)) {
            BL_LOG_ERROR("Could not initialize window module");
            return false;
        }
        if (!graphicsModule->initialize(appConfig)) {
            BL_LOG_ERROR("Could not initialize graphics module");
            return false;
        }
        if (!gameModule->initialize(appConfig)) {
            BL_LOG_ERROR("Could not initialize game module");
            return false;
        }
        return true;
    }

    void App::terminate() const {
        gameModule->terminate();
        graphicsModule->terminate();
        windowModule->terminate();
        systemModule->terminate();
    }
}
