#include "GraphicsModule.h"
#include "system/Log.h"

namespace Blink {
    GraphicsModule::GraphicsModule(SystemModule* systemModule)
            : systemModule(systemModule),
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

    bool GraphicsModule::initialize(const Config& config) const {
        if (!vulkan->initialize(config)) {
            BL_LOG_ERROR("Could not initialize Vulkan");
            return false;
        }
        if (!vulkanPhysicalDevice->initialize()) {
            BL_LOG_ERROR("Could not initialize Vulkan physical device");
            return false;
        }
        if (!vulkanDevice->initialize(config)) {
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
