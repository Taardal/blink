#include "Game.h"

namespace Blink {
    Game::Game(Window *window, Renderer *renderer)
            : window(window),
              renderer(renderer) {
    }

    void Game::run() {
        BL_LOG_INFO("Running application...");
        while (!window->shouldClose()) {

            renderer->beginFrame();

            Quad quad{};
            quad.position = { 0.0f, 0.0f, 0.0f };
            quad.size = { 1.0f, 1.0f };
            quad.color = { 0.0f, 1.0f, 0.0f, 0.0f };
            renderer->submitQuad(quad);

            renderer->endFrame();

            window->onUpdate();
        }
    }

    void Game::onUpdate() const {

    }

    void Game::onRender() const {

    }
}
