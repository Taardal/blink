#include "GraphicsModule.h"
#include "system/Log.h"

namespace Blink {
    GraphicsModule::GraphicsModule(SystemModule* systemModule, WindowModule* windowModule)
            : systemModule(systemModule),
              vulkan(new Vulkan(windowModule->getWindow())),
              physicalDevice(new VulkanPhysicalDevice(vulkan)),
              device(new VulkanDevice(physicalDevice)),
              swapChain(new VulkanSwapChain(device, physicalDevice, vulkan, windowModule->getWindow())),
              renderPass(new VulkanRenderPass(swapChain, device)),
              vertexShader(new VulkanShader(device)),
              fragmentShader(new VulkanShader(device)),
              graphicsPipeline(new VulkanGraphicsPipeline(vertexShader, fragmentShader, renderPass, swapChain, device)),
              commandPool(new VulkanCommandPool(device, physicalDevice)),
              renderer(new Renderer(commandPool, graphicsPipeline, renderPass, swapChain, device, physicalDevice, windowModule->getWindow())) {
    }

    GraphicsModule::~GraphicsModule() {
        delete renderer;
        delete commandPool;
        delete graphicsPipeline;
        delete fragmentShader;
        delete vertexShader;
        delete renderPass;
        delete swapChain;
        delete device;
        delete physicalDevice;
        delete vulkan;
    }

    Renderer* GraphicsModule::getRenderer() const {
        return renderer;
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
        FileSystem* fileSystem = systemModule->getFileSystem();
        if (!vertexShader->initialize(fileSystem->readBytes("shaders/shader.vert.spv"))) {
            BL_LOG_ERROR("Could not initialize vertex shader");
            return false;
        }
        if (!fragmentShader->initialize(fileSystem->readBytes("shaders/shader.frag.spv"))) {
            BL_LOG_ERROR("Could not initialize fragment shader");
            return false;
        }
        if (!graphicsPipeline->initialize()) {
            BL_LOG_ERROR("Could not initialize graphics pipeline");
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
        graphicsPipeline->terminate();
        fragmentShader->terminate();
        vertexShader->terminate();
        renderPass->terminate();
        swapChain->terminate();
        device->terminate();
        physicalDevice->terminate();
        vulkan->terminate();
    }
}
