#include "App.h"

#include "window/KeyEvent.h"

namespace Blink {
    App::App(const AppConfig& config) : config(config) {
        try {
            initialize();
            BL_LOG_INFO("Initializing...");
            initialized = true;
        } catch (const Error& e) {
            BL_LOG_CRITICAL("Initialization error");
            e.printStack();
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Initialization error: {}", e.what());
        }
    }

    void App::initialize() {
        fileSystem = new FileSystem();
        window = new Window(config);
        window->setEventListener([this](Event& event) {
            onEvent(event);
        });
        keyboard = new Keyboard(window);
        renderer = new Renderer(config, fileSystem, window);
        luaEngine = new LuaEngine(keyboard);
        camera = new Camera(window, keyboard);
        scene = new Scene(keyboard, luaEngine);
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
        renderer->waitUntilIdle();
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