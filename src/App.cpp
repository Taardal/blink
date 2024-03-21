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
        VulkanConfig vulkanConfig{};
        vulkanConfig.applicationName = appConfig.windowTitle;
        vulkanConfig.engineName = appConfig.windowTitle;
        vulkanConfig.validationLayersEnabled = Environment::isDebug();
        vulkan = new Vulkan(vulkanConfig, window);
        physicalDevice = new VulkanPhysicalDevice(vulkan);
        device = new VulkanDevice(physicalDevice);
        swapChain = new VulkanSwapChain(device, physicalDevice, vulkan, window);
        renderPass = new VulkanRenderPass(swapChain, device);
        commandPool = new VulkanCommandPool(device, physicalDevice);
        renderer = new Renderer(fileSystem, window, physicalDevice, device, swapChain, renderPass, commandPool);
        luaEngine = new LuaEngine(keyboard);
        camera = new Camera(window, keyboard);
        scene = new Scene(keyboard, luaEngine);
    }

    App::~App() {
        delete scene;
        delete camera;
        delete luaEngine;
        delete renderer;
        delete commandPool;
        delete renderPass;
        delete swapChain;
        delete device;
        delete physicalDevice;
        delete vulkan;
        delete keyboard;
        delete window;
        delete fileSystem;
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

    void App::onEvent(Event& event) const {
        if (event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == Key::Escape) {
            window->setShouldClose(true);
            return;
        }
        renderer->onEvent(event);
        scene->onEvent(event);
    }
}
