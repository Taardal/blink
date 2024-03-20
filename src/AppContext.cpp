#include "AppContext.h"

namespace Blink {
    AppContext::AppContext(
        const AppConfig& appConfig
    ) : fileSystem(new FileSystem()),

        window(new Window()),
        keyboard(new Keyboard(window)),

        vulkan(new Vulkan(window)),
        physicalDevice(new VulkanPhysicalDevice(vulkan)),
        device(new VulkanDevice(physicalDevice)),
        swapChain(new VulkanSwapChain(device, physicalDevice, vulkan, window)),
        renderPass(new VulkanRenderPass(swapChain, device)),
        commandPool(new VulkanCommandPool(device, physicalDevice)),
        renderer(new Renderer(fileSystem, window, physicalDevice, device, swapChain, renderPass, commandPool)),

        luaEngine(new LuaEngine(keyboard)),

        camera(new Camera(window, keyboard)),
        scene(new Scene(keyboard, luaEngine)),
        app(new App(window, renderer, camera, scene))
    {
        if (!window->initialize(appConfig)) {
            throw std::runtime_error("Could not initialize window");
        }
        if (!keyboard->initialize()) {
            throw std::runtime_error("Could not initialize keyboard");
        }
        VulkanConfig vulkanConfig{};
        vulkanConfig.applicationName = appConfig.windowTitle;
        vulkanConfig.engineName = appConfig.windowTitle;
        vulkanConfig.validationLayersEnabled = Environment::isDebug();
        if (!vulkan->initialize(vulkanConfig)) {
            throw std::runtime_error("Could not initialize Vulkan");
        }
        if (!physicalDevice->initialize()) {
            throw std::runtime_error("Could not initialize Vulkan physical device");
        }
        if (!device->initialize()) {
            throw std::runtime_error("Could not initialize Vulkan device");
        }
        if (!swapChain->initialize()) {
            throw std::runtime_error("Could not initialize Vulkan swap chain");
        }
        if (!renderPass->initialize()) {
            throw std::runtime_error("Could not initialize Vulkan render pass");
        }
        if (!commandPool->initialize()) {
            throw std::runtime_error("Could not initialize command pool");
        }
        if (!renderer->initialize()) {
            throw std::runtime_error("Could not initialize renderer");
        }

        if (!luaEngine->initialize()) {
            throw std::runtime_error("Could not initialize lua engine");
        }

        if (!camera->initialize()) {
            throw std::runtime_error("Could not initialize camera");
        }
        if (!scene->initialize()) {
            throw std::runtime_error("Could not initialize scene");
        }
    }

    AppContext::~AppContext() {
        scene->terminate();
        camera->terminate();

        luaEngine->terminate();

        renderer->terminate();
        commandPool->terminate();
        renderPass->terminate();
        swapChain->terminate();
        device->terminate();
        physicalDevice->terminate();
        vulkan->terminate();

        keyboard->terminate();
        window->terminate();

        delete app;
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
}
