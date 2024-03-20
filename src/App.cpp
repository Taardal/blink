#include "App.h"
#include "AppContext.h"

namespace Blink {
    void runApp(const AppConfig& appConfig) {
        Log::SetLevel(appConfig.logLevel);
        AppContext* appContext;
        try {
            appContext = new AppContext(appConfig);
        } catch (const std::exception& e) {
            appContext = nullptr;
            BL_LOG_CRITICAL("Could not initialize app: {}", e.what());
        }
        if (appContext == nullptr) {
            return;
        }
        App* app = appContext->app;
        try {
            app->run();
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Could not run app: {}", e.what());
        }
        delete appContext;
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
