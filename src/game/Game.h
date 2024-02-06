#pragma once

#include "Camera.h"
#include "window/Window.h"
#include "graphics/Renderer.h"

namespace Blink {
    class Game {
    private:
        Camera* camera;
        Window* window;
        Renderer* renderer;
        double lastFrameTime = 0.0;
        glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    public:
        Game(Camera* camera, Window* window, Renderer* renderer);

        void run();

    private:
        void onUpdate() const;

        void onRender() const;
    };
}
