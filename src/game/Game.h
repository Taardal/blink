#pragma once

#include "Scene.h"
#include "Camera.h"
#include "window/Window.h"
#include "graphics/Renderer.h"

namespace Blink {
    class Game {
    private:
        Scene* scene;
        Camera* camera;
        Window* window;
        Renderer* renderer;
        double lastFrameTime = 0.0;
        glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    public:
        Game(Scene* scene, Camera* camera, Window* window, Renderer* renderer);

        bool initialize() const;

        void terminate() const;

        void run();
    };
}
