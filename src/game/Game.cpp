#include "Game.h"
#include <chrono>

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
        BL_LOG_INFO("Running application...");
        while (!window->shouldClose()) {
            double timestep = window->update();
            camera->update(timestep);

            glm::mat4 playerModel = scene->update(timestep);

            Frame frame{};
            frame.model = playerModel;
            frame.view = camera->getViewMatrix();
            frame.projection = camera->getProjectionMatrix();

            renderer->render(frame);
        }
    }
}
