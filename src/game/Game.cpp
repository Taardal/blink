#include "Game.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Blink {
    Game::Game(
        Camera* camera,
        Window* window,
        Renderer* renderer
    ) : camera(camera),
        window(window),
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

            double time = glfwGetTime();
            double timestep = time - lastFrameTime;
            lastFrameTime = time;

            camera->onUpdate(timestep);

            // TRANSLATION
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), playerPosition);

            // ROTATION
            // const float angle = glm::radians(90.0f);
            // const glm::vec3 axis(0.0f, 0.0f, 1.0f);
            // glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, axis);
            glm::mat4 rotation = glm::mat4(1.0f);

            // SCALE
            //glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            glm::mat4 scale = glm::mat4(1.0f);

            Frame frame{};
            frame.model = translation * rotation * scale;
            frame.view = camera->getViewMatrix();
            frame.projection = camera->getProjectionMatrix();

            renderer->onRender(frame);
        }

        renderer->onComplete();
    }

    void Game::onUpdate() const {
    }

    void Game::onRender() const {
    }
}
