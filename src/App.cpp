#include "App.h"

#include "window/KeyEvent.h"

namespace Blink {
    void runApp(const AppConfig& config) {
        Log::SetLevel(config.logLevel);
        App* app;
        try {
            BL_LOG_INFO("Initializing app...");
            app = new App(config);
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Initialization error: {}", e.what());
            return;
        }
        try {
            BL_LOG_INFO("Running app...");
            app->run();
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Runtime error: {}", e.what());
        }
        BL_LOG_INFO("Stopping app...");
        delete app;
    }
}

namespace Blink {
    App::App(const AppConfig& appConfig) {
        fileSystem = new FileSystem();
        window = new Window(appConfig);
        window->setEventListener([this](Event& event) {
            this->onEvent(event);
        });
        keyboard = new Keyboard(window);
        renderer = new Renderer(appConfig, fileSystem, window);
        luaEngine = new LuaEngine(keyboard);
        camera = new Camera(window, keyboard);
        scene = new Scene(keyboard, luaEngine);
    }

    App::~App() {
        delete scene;
        delete camera;
        delete luaEngine;
        delete renderer;
        delete keyboard;
        delete window;
        delete fileSystem;
    }

    void App::run() {
        while (!window->shouldClose()) {
            double time = window->update();
            double timestep = time - lastTime;
            lastTime = time;

            camera->update(timestep);

            glm::mat4 playerModel = scene->update(timestep);

            Frame frame{};
            frame.model = playerModel;
            frame.view = camera->getViewMatrix();
            frame.projection = camera->getProjectionMatrix();

            renderer->render(frame);

            fps++;
            fpsUpdateTimestep += timestep;
            if (fpsUpdateTimestep >= 1.0) {
                BL_LOG_DEBUG("FPS [{}]", fps);
                fps = 0;
                fpsUpdateTimestep = 0;
            }
        }
    }

    void App::onEvent(Event& event) const {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Escape) {
            window->setShouldClose(true);
            return;
        }
        renderer->onEvent(event);
        scene->onEvent(event);
    }
}
