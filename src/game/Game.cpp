#include "Game.h"

namespace Blink {
    Game::Game(
        Scene* scene,
        Camera* camera,
        Window* window,
        Renderer* renderer
    ) : scene(scene),
        camera(camera),
        window(window),
        renderer(renderer) {
    }

    bool Game::initialize() const {
        return true;
    }

    void Game::terminate() const {
    }

    void Game::run() {

    }
}
