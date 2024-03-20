#pragma once

#include "AppConfig.h"
#include "window/Window.h"
#include "graphics/Renderer.h"
#include "game/Camera.h"
#include "game/Scene.h"

namespace Blink {
    void runApp(const AppConfig& appConfig);
}

namespace Blink {
    class App {
    private:
        Window* window;
        Renderer* renderer;
        Camera* camera;
        Scene* scene;

    public:
        App(
            Window* window,
            Renderer* renderer,
            Camera* camera,
            Scene* scene
        );

        void run() const;
    };
}
