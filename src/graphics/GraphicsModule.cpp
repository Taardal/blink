#include "GraphicsModule.h"
#include "system/Log.h"

namespace Blink {
    GraphicsModule::GraphicsModule(SystemModule* systemModule, WindowModule* windowModule)
            : systemModule(systemModule),
              windowModule(windowModule),
              vulkan(new Vulkan()),
              vulkanPhysicalDevice(new VulkanPhysicalDevice(vulkan)),
              vulkanDevice(new VulkanDevice(vulkanPhysicalDevice)),
              indexBuffer(new IndexBuffer()),
              vertexBuffer(new VertexBuffer()),
              shader(new Shader()),
              whiteTexture(new Texture()),
              renderer(new Renderer(indexBuffer, vertexBuffer, shader, whiteTexture)) {
    }

    GraphicsModule::~GraphicsModule() {
        delete renderer;
        delete whiteTexture;
        delete shader;
        delete vertexBuffer;
        delete indexBuffer;
        delete vulkanDevice;
        delete vulkanPhysicalDevice;
        delete vulkan;
    }

    Renderer* GraphicsModule::getRenderer() const {
        return renderer;
    }

    bool GraphicsModule::initialize(const AppConfig& appConfig) const {
        Window* window = windowModule->getWindow();
        if (!window->isVulkanSupported()) {
            BL_LOG_ERROR("Vulkan is not supported");
            return false;
        }
        VulkanConfig vulkanConfig{};
        vulkanConfig.applicationName = appConfig.windowTitle;
        vulkanConfig.engineName = appConfig.windowTitle;
        vulkanConfig.requiredExtensions = window->getRequiredVulkanExtensions();
        if (vulkanConfig.requiredExtensions.empty()) {
            BL_LOG_ERROR("Could not get required Vulkan instance extensions");
            return false;
        }
        if (Environment::isMacOS()) {
            vulkanConfig.requiredExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        }
        if (Environment::isDebug()) {
            vulkanConfig.requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            vulkanConfig.validationLayers.push_back("VK_LAYER_KHRONOS_validation");
        }
        if (!vulkan->initialize(vulkanConfig)) {
            BL_LOG_ERROR("Could not initialize Vulkan");
            return false;
        }

        VulkanPhysicalDeviceConfig physicalDeviceConfig{};
        physicalDeviceConfig.requiredExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        if (Environment::isMacOS()) {
            physicalDeviceConfig.requiredExtensions.push_back("VK_KHR_portability_subset");
        }
        if (!vulkanPhysicalDevice->initialize(physicalDeviceConfig)) {
            BL_LOG_ERROR("Could not initialize Vulkan physical device");
            return false;
        }
        if (!vulkanDevice->initialize(appConfig)) {
            BL_LOG_ERROR("Could not initialize Vulkan device");
            return false;
        }
        if (!indexBuffer->initialize()) {
            BL_LOG_ERROR("Could not initialize index buffer");
            return false;
        }
        if (!vertexBuffer->initialize()) {
            BL_LOG_ERROR("Could not initialize vertex buffer");
            return false;
        }
        FileSystem* fileSystem = systemModule->getFileSystem();
        std::string vertexShaderSrc = fileSystem->readFile("res/shaders/texture.vert.glsl");
        if (vertexShaderSrc.empty()) {
            BL_LOG_ERROR("Could not read vertex shader src");
            return false;
        }
        std::string fragmentShaderSrc = fileSystem->readFile("res/shaders/texture.frag.glsl");
        if (fragmentShaderSrc.empty()) {
            BL_LOG_ERROR("Could not read fragment shader src");
            return false;
        }
        if (!shader->initialize(vertexShaderSrc, fragmentShaderSrc)) {
            BL_LOG_ERROR("Could not initialize shader");
            return false;
        }
        if (!whiteTexture->initialize()) {
            BL_LOG_ERROR("Could not initialize white texture");
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
        whiteTexture->terminate();
        shader->terminate();
        vertexBuffer->terminate();
        indexBuffer->terminate();
        vulkanDevice->terminate();
        vulkanPhysicalDevice->terminate();
        vulkan->terminate();
    }
}
