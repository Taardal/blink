#pragma once

#include "Vulkan.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "system/SystemModule.h"
#include "window/WindowModule.h"

namespace Blink {
    class GraphicsModule {
    private:
        SystemModule* systemModule;
        WindowModule* windowModule;
        Vulkan* vulkan;
        VulkanPhysicalDevice* vulkanPhysicalDevice;
        VulkanDevice* vulkanDevice;
        IndexBuffer* indexBuffer;
        VertexBuffer* vertexBuffer;
        Shader* shader;
        Texture* whiteTexture;
        Renderer *renderer;

    public:
        GraphicsModule(SystemModule* systemModule, WindowModule* windowModule);

        ~GraphicsModule();

        Renderer *getRenderer() const;

        bool initialize(const AppConfig& appConfig) const;

        void terminate() const;
    };
}
