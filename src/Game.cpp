#include "Game.h"

namespace Blink {
    Game::Game(Window *window, Renderer *renderer)
            : window(window),
              renderer(renderer) {
    }

    void Game::run() {
        BL_LOG_INFO("Running application...");

        window->setResizeListener([this](uint32_t width, uint32_t height) {
            renderer->onResize(width, height);
        });
        window->setMinimizeListener([this](bool minimized) {
            renderer->onMinimize(minimized);
        });

        while (!window->shouldClose()) {
            window->onUpdate();
            renderer->onRender();
        }

        renderer->onComplete();
    }

    void Game::onUpdate() const {

    }

    void Game::onRender() const {

    }
}
