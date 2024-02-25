#include "pch.h"
#include "GraphicsModule.h"

namespace Blink {
    GraphicsModule::GraphicsModule(SystemModule* systemModule, WindowModule* windowModule)
            : vulkan(new Vulkan(windowModule->window)),
              physicalDevice(new VulkanPhysicalDevice(vulkan)),
              device(new VulkanDevice(physicalDevice)),
              swapChain(new VulkanSwapChain(device, physicalDevice, vulkan, windowModule->window)),
              renderPass(new VulkanRenderPass(swapChain, device)),
              commandPool(new VulkanCommandPool(device, physicalDevice)),
              renderer(new Renderer(systemModule->fileSystem, windowModule->window, physicalDevice, device, swapChain, renderPass, commandPool)) {
    }

    GraphicsModule::~GraphicsModule() {
        delete renderer;
        delete commandPool;
        delete renderPass;
        delete swapChain;
        delete device;
        delete physicalDevice;
        delete vulkan;
    }

    bool GraphicsModule::initialize(const AppConfig& appConfig) {
        VulkanConfig vulkanConfig{};
        vulkanConfig.applicationName = appConfig.windowTitle;
        vulkanConfig.engineName = appConfig.windowTitle;
        vulkanConfig.validationLayersEnabled = Environment::isDebug();
        if (!vulkan->initialize(vulkanConfig)) {
            BL_LOG_ERROR("Could not initialize Vulkan");
            return false;
        }
        if (!physicalDevice->initialize()) {
            BL_LOG_ERROR("Could not initialize Vulkan physical device");
            return false;
        }
        if (!device->initialize()) {
            BL_LOG_ERROR("Could not initialize Vulkan device");
            return false;
        }
        if (!swapChain->initialize()) {
            BL_LOG_ERROR("Could not initialize Vulkan swap chain");
            return false;
        }
        if (!renderPass->initialize()) {
            BL_LOG_ERROR("Could not initialize Vulkan render pass");
            return false;
        }
        if (!commandPool->initialize()) {
            BL_LOG_ERROR("Could not initialize command pool");
            return false;
        }
        if (!renderer->initialize()) {
            BL_LOG_ERROR("Could not initialize renderer");
            return false;
        }
        return true;
    }

    void GraphicsModule::terminate() const {
        renderer->terminate();
        commandPool->terminate();
        renderPass->terminate();
        swapChain->terminate();
        device->terminate();
        physicalDevice->terminate();
        vulkan->terminate();
    }
}
