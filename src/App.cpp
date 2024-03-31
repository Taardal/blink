#include "App.h"

#include "window/KeyEvent.h"

namespace Blink {
    void runApp(const AppConfig& config) {
        Log::setLevel(config.logLevel);
        App* app = new App(config);
        app->run();
        delete app;
    }
}

namespace Blink {
    App::App(const AppConfig& config) : config(config) {
        try {
            BL_LOG_INFO("Initializing...");
            initialize();
            initialized = true;
        } catch (const Error& e) {
            BL_LOG_CRITICAL("Initialization error");
            e.printStack();
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Initialization error: {}", e.what());
        }
    }

    void App::initialize() {
        BL_TRY(fileSystem = new FileSystem());
        BL_TRY(window = new Window(config));
        window->setEventListener([this](Event& event) {
            this->onEvent(event);
        });
        BL_TRY(keyboard = new Keyboard(window));
        BL_TRY(renderer = new Renderer(config, fileSystem, window));
        BL_TRY(luaEngine = new LuaEngine(keyboard));
        BL_TRY(camera = new Camera(window, keyboard));
        BL_TRY(scene = new Scene(keyboard, luaEngine));
    }

    App::~App() {
        BL_LOG_INFO("Terminating...");
        delete scene;
        delete camera;
        delete luaEngine;
        delete renderer;
        delete keyboard;
        delete window;
        delete fileSystem;
    }

    void App::run() {
        if (!initialized) {
            return;
        }
        try {
            BL_LOG_INFO("Running...");
            while (!window->shouldClose()) {
                update();
            }
        } catch (const Error& e) {
            BL_LOG_CRITICAL("Runtime error");
            e.printStack();
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Runtime error: {}", e.what());
        }
    }

    void App::update() {
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

    void App::onEvent(Event& event) const {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Escape) {
            window->setShouldClose(true);
            return;
        }
        renderer->onEvent(event);
        scene->onEvent(event);
    }
}