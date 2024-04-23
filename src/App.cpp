#include "App.h"

#include "graphics/ResourceLoader.h"
#include "window/KeyEvent.h"

namespace Blink {
    App::App(const AppConfig& config) : config(config) {
        try {
            initialize();
            BL_LOG_INFO("Initializing...");
            initialized = true;
        } catch (const Error& e) {
            BL_LOG_CRITICAL("Initialization error");
            e.printStacktrace();
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Initialization error: {}", e.what());
        }
    }

    void App::initialize() {
        BL_EXECUTE_THROW(fileSystem = new FileSystem());

        WindowConfig windowConfig{};
        windowConfig.title = config.name;
        windowConfig.width = config.windowWidth;
        windowConfig.height = config.windowHeight;
        windowConfig.resizable = config.windowResizable;
        windowConfig.maximized = config.windowMaximized;
        windowConfig.onEvent = [this](Event& event) {
            onEvent(event);
        };
        BL_EXECUTE_THROW(window = new Window(windowConfig));

        KeyboardConfig keyboardConfig{};
        keyboardConfig.window = window;
        BL_EXECUTE_THROW(keyboard = new Keyboard(keyboardConfig));

        ResourceLoaderConfig resourceLoaderConfig{};
        resourceLoaderConfig.fileSystem = fileSystem;
        BL_EXECUTE_THROW(resourceLoader = new ResourceLoader(resourceLoaderConfig));

        RendererConfig rendererConfig{};
        rendererConfig.applicationName = config.name;
        rendererConfig.engineName = config.name;
        rendererConfig.fileSystem = fileSystem;
        rendererConfig.window = window;
        rendererConfig.resourceLoader = resourceLoader;
        BL_EXECUTE_THROW(renderer = new Renderer(rendererConfig));

        LuaEngineConfig luaEngineConfig{};
        luaEngineConfig.keyboard = keyboard;
        BL_EXECUTE_THROW(luaEngine = new LuaEngine(luaEngineConfig));

        CameraConfig cameraConfig{};
        cameraConfig.window = window;
        cameraConfig.keyboard = keyboard;
        BL_EXECUTE_THROW(camera = new Camera(cameraConfig));

        SceneConfig sceneConfig{};
        sceneConfig.keyboard = keyboard;
        sceneConfig.renderer = renderer;
        sceneConfig.luaEngine = luaEngine;
        sceneConfig.camera = camera;
        BL_EXECUTE_THROW(scene = new Scene(sceneConfig));
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
                render();
            }
        } catch (const Error& e) {
            BL_LOG_CRITICAL("Runtime error");
            e.printStacktrace();
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Runtime error: {}", e.what());
        }
        renderer->waitUntilIdle();
    }

    void App::update() {
        double time = window->update();
        double timestep = time - lastTime;
        lastTime = time;

        fps++;
        fpsUpdateTimestep += timestep;
        if (fpsUpdateTimestep >= 1.0) {
            BL_LOG_DEBUG("FPS [{}]", fps);
            fps = 0;
            fpsUpdateTimestep = 0;
        }

        camera->update(timestep);
        scene->update(timestep);
    }

    void App::render() const {
        if (!renderer->beginFrame()) {
            return;
        }
        scene->render();
        renderer->endFrame();
    }

    void App::onEvent(Event& event) const {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Escape) {
            window->setShouldClose(true);
            return;
        }
        renderer->onEvent(event);
        camera->onEvent(event);
        scene->onEvent(event);
    }
}