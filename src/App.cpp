#include "App.h"
#include "DependencyContainer.h"

namespace Blink {
    void runApp(const AppConfig& config) {
        Log::SetLevel(config.logLevel);
        DependencyContainer* dependencyContainer;
        try {
            dependencyContainer = new DependencyContainer(config);
        } catch (const std::exception& e) {
            dependencyContainer = nullptr;
            BL_LOG_CRITICAL("Initialization error: {}", e.what());
        }
        if (dependencyContainer == nullptr) {
            return;
        }
        App* app = dependencyContainer->app;
        try {
            app->run();
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Runtime error: {}", e.what());
        }
        delete dependencyContainer;
    }
}

namespace Blink {
    App::App(
        Window* window,
        Renderer* renderer,
        Camera* camera,
        Scene* scene
    ) : window(window),
        renderer(renderer),
        camera(camera),
        scene(scene) {
    }

    void App::run() const {
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
