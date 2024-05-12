#include "App.h"

#include "graphics/MeshManager.h"
#include "window/KeyEvent.h"

namespace Blink {
    App::App(const AppConfig& config) : config(config), secondsPerFrame(1.0 / (double) config.fps) {
        try {
            BL_LOG_INFO("Initializing...");
            initialize();
            state = AppState::Initialized;
        } catch (const Error& e) {
            BL_LOG_CRITICAL("Initialization error");
            e.printStacktrace();
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Initialization error: {}", e.what());
        }
    }

    App::~App() {
        BL_LOG_INFO("Terminating...");
        terminate();
    }

    void App::run() {
        if (state == AppState::None) {
            return;
        }
        try {
            BL_LOG_INFO("Running...");
            while (!window->shouldClose()) {
                update();
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
        secondsSinceLastFrame += timestep;
        secondsSinceLastFrame = std::min(secondsSinceLastFrame, 0.1);
        secondsSinceLastStatsUpdate += timestep;

        if (state == AppState::Paused) {
            return;
        }

        while (secondsSinceLastFrame >= secondsPerFrame) {
            scene->update(timestep);

            fps++;
            secondsSinceLastFrame -= secondsPerFrame;
        }

        if (!renderer->beginFrame()) {
            return;
        }
        scene->render();
        renderer->endFrame();

        if (secondsSinceLastStatsUpdate >= 1) {
            BL_LOG_DEBUG("FPS [{}]", fps);
            fps = 0;
            secondsSinceLastStatsUpdate = 0;
        }
    }

    void App::onEvent(Event& event) {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Escape) {
            window->setShouldClose(true);
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::P) {
            state = AppState::Paused;
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::O) {
            state = AppState::Running;
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::F12) {
            renderer->waitUntilIdle();

            delete scene;
            delete sceneCamera;
            delete luaEngine;

            LuaEngineConfig luaEngineConfig{};
            luaEngineConfig.keyboard = keyboard;
            BL_EXECUTE_THROW(luaEngine = new LuaEngine(luaEngineConfig));

            SceneCameraConfig cameraConfig{};
            cameraConfig.window = window;
            cameraConfig.keyboard = keyboard;
            cameraConfig.mouse = mouse;
            BL_EXECUTE_THROW(sceneCamera = new SceneCamera(cameraConfig));

            SceneConfig sceneConfig{};
            sceneConfig.keyboard = keyboard;
            sceneConfig.meshManager = meshManager;
            sceneConfig.renderer = renderer;
            sceneConfig.luaEngine = luaEngine;
            sceneConfig.sceneCamera = sceneCamera;
            BL_EXECUTE_THROW(scene = new Scene(sceneConfig));

            return;
        }
        renderer->onEvent(event);
        scene->onEvent(event);
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

        MouseConfig mouseConfig{};
        mouseConfig.window = window;
        BL_EXECUTE_THROW(mouse = new Mouse(mouseConfig));

        VulkanAppConfig vulkanAppConfig{};
        vulkanAppConfig.window = window;
        vulkanAppConfig.applicationName = config.name;
        vulkanAppConfig.engineName = config.name;
#ifdef BL_DEBUG
        vulkanAppConfig.validationLayersEnabled = true;
#endif
        BL_EXECUTE_THROW(vulkanApp = new VulkanApp(vulkanAppConfig));

        VulkanPhysicalDeviceConfig physicalDeviceConfig{};
        physicalDeviceConfig.vulkanApp = vulkanApp;
        BL_EXECUTE_THROW(vulkanPhysicalDevice = new VulkanPhysicalDevice(physicalDeviceConfig));

        VulkanDeviceConfig deviceConfig{};
        deviceConfig.physicalDevice = vulkanPhysicalDevice;
        BL_EXECUTE_THROW(vulkanDevice = new VulkanDevice(deviceConfig));

        ShaderManagerConfig shaderManagerConfig{};
        shaderManagerConfig.fileSystem = fileSystem;
        shaderManagerConfig.device = vulkanDevice;
        BL_EXECUTE_THROW(shaderManager = new ShaderManager(shaderManagerConfig));

        MeshManagerConfig meshManagerConfig{};
        meshManagerConfig.fileSystem = fileSystem;
        meshManagerConfig.device = vulkanDevice;
        BL_EXECUTE_THROW(meshManager = new MeshManager(meshManagerConfig));

        RendererConfig rendererConfig{};
        rendererConfig.fileSystem = fileSystem;
        rendererConfig.window = window;
        rendererConfig.meshManager = meshManager;
        rendererConfig.shaderManager = shaderManager;
        rendererConfig.vulkanApp = vulkanApp;
        rendererConfig.device = vulkanDevice;
        BL_EXECUTE_THROW(renderer = new Renderer(rendererConfig));

        LuaEngineConfig luaEngineConfig{};
        luaEngineConfig.keyboard = keyboard;
        BL_EXECUTE_THROW(luaEngine = new LuaEngine(luaEngineConfig));

        SceneCameraConfig cameraConfig{};
        cameraConfig.window = window;
        cameraConfig.keyboard = keyboard;
        cameraConfig.mouse = mouse;
        BL_EXECUTE_THROW(sceneCamera = new SceneCamera(cameraConfig));

        SceneConfig sceneConfig{};
        //sceneConfig.name = config.scene;
        sceneConfig.keyboard = keyboard;
        sceneConfig.meshManager = meshManager;
        sceneConfig.renderer = renderer;
        sceneConfig.luaEngine = luaEngine;
        sceneConfig.sceneCamera = sceneCamera;
        BL_EXECUTE_THROW(scene = new Scene(sceneConfig));


    }

    void App::terminate() const {
        delete scene;
        delete sceneCamera;
        delete luaEngine;
        delete renderer;
        delete meshManager;
        delete shaderManager;
        delete vulkanDevice;
        delete vulkanPhysicalDevice;
        delete vulkanApp;
        delete keyboard;
        delete window;
        delete fileSystem;
    }
}
