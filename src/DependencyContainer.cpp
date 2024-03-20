#include "DependencyContainer.h"

namespace Blink {
    DependencyContainer::DependencyContainer(const AppConfig& appConfig) {
        fileSystem = new FileSystem();
        window = new Window(appConfig);
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
        app = new App(window, renderer, camera, scene);
    }

    DependencyContainer::~DependencyContainer() {
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
