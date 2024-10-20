#include "App.h"
#include "graphics/MeshManager.h"
#include "system/Error.h"
#include "window/KeyEvent.h"

namespace Blink {
    App::App(const AppConfig& config) : config(config) {
        try {
            BL_LOG_INFO("Initializing...");
            initialize();
            initialized = true;
        } catch (const Error& e) {
            BL_LOG_CRITICAL("Initialization error");
            e.printStacktrace();
            initialized = false;
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Initialization error: {}", e.what());
            initialized = false;
        }
    }

    App::~App() {
        BL_LOG_INFO("Terminating...");
        terminate();
    }

    void App::run() {
        if (!initialized) {
            return;
        }
        try {
            BL_LOG_INFO("Running...");
            running = true;
            gameLoop();
        } catch (const Error& e) {
            BL_LOG_CRITICAL("Runtime error");
            e.printStacktrace();
        } catch (const std::exception& e) {
            BL_LOG_CRITICAL("Runtime error: {}", e.what());
        }
        running = false;
        renderer->waitUntilIdle();
    }

    void App::gameLoop() const {
        constexpr double oneSecond = 1.0;
        double lastTime = window->getTime();
        double statisticsUpdateLag = 0.0;
        uint32_t ups = 0;
        uint32_t fps = 0;
        while (running) {
            double time = window->update();
            double timestep = std::min(time - lastTime, oneSecond);
            lastTime = time;
            if (!paused) {
                scene->update(timestep);
                ups++;
            }
            if (renderer->beginFrame()) {
                scene->render();
                renderer->endFrame();
                fps++;
            }
#ifdef BL_DEBUG
            statisticsUpdateLag += timestep;
            if (statisticsUpdateLag >= oneSecond) {
                std::stringstream ss;
                ss << "FPS: " << fps << ", UPS: " << ups;
                std::string title = ss.str();
                window->setTitle(title.c_str());
                ups = 0;
                fps = 0;
                statisticsUpdateLag = 0;
            }
#endif
        }
    }

    void App::onEvent(Event& event) {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Escape) {
            running = false;
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::P) {
            paused = true;
            return;
        }
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::O) {
            paused = false;
            return;
        }
        if (event.type == EventType::KeyPressed) {
            auto key = event.as<KeyPressedEvent>().keyCode;
            auto f1Key = (uint32_t) Key::F1;
            if (key >= f1Key) {
                int32_t sceneIndex = key - f1Key;
                if (sceneIndex > -1 && sceneIndex < config.scenes.size()) {
                    setScene(config.scenes[sceneIndex]);
                    return;
                }
            }
        }
        renderer->onEvent(event);
        scene->onEvent(event);
    }

    void App::setScene(const std::string& scenePath) {
        BL_LOG_INFO("Setting scene [{}]", scenePath);

        if (scene != nullptr) {
            renderer->waitUntilIdle();
            delete scene;
        }

        SceneConfig sceneConfig{};
        sceneConfig.scene = scenePath;
        sceneConfig.keyboard = keyboard;
        sceneConfig.meshManager = meshManager;
        sceneConfig.skyboxManager = skyboxManager;
        sceneConfig.renderer = renderer;
        sceneConfig.luaEngine = luaEngine;
        sceneConfig.sceneCamera = sceneCamera;

        BL_EXECUTE_THROW(scene = new Scene(sceneConfig));
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

        SkyboxManagerConfig skyboxManagerConfig{};
        skyboxManagerConfig.fileSystem = fileSystem;
        skyboxManagerConfig.device = vulkanDevice;
        BL_EXECUTE_THROW(skyboxManager = new SkyboxManager(skyboxManagerConfig));

        RendererConfig rendererConfig{};
        rendererConfig.fileSystem = fileSystem;
        rendererConfig.window = window;
        rendererConfig.vulkanApp = vulkanApp;
        rendererConfig.device = vulkanDevice;
        rendererConfig.meshManager = meshManager;
        rendererConfig.shaderManager = shaderManager;
        rendererConfig.skyboxManager = skyboxManager;
        BL_EXECUTE_THROW(renderer = new Renderer(rendererConfig));

        SceneCameraConfig cameraConfig{};
        cameraConfig.window = window;
        cameraConfig.keyboard = keyboard;
        cameraConfig.mouse = mouse;
        BL_EXECUTE_THROW(sceneCamera = new SceneCamera(cameraConfig));

        LuaEngineConfig luaEngineConfig{};
        luaEngineConfig.keyboard = keyboard;
        luaEngineConfig.sceneCamera = sceneCamera;
        luaEngineConfig.window = window;
        BL_EXECUTE_THROW(luaEngine = new LuaEngine(luaEngineConfig));

        BL_ASSERT_THROW(!config.scenes.empty());
        setScene(config.scenes[0]);
    }

    void App::terminate() const {
        delete scene;
        delete sceneCamera;
        delete luaEngine;
        delete renderer;
        delete skyboxManager;
        delete meshManager;
        delete shaderManager;
        delete vulkanDevice;
        delete vulkanPhysicalDevice;
        delete vulkanApp;
        delete mouse;
        delete keyboard;
        delete window;
        delete fileSystem;
    }
}
