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

namespace Blink {
    class GraphicsModule {
    private:
        SystemModule* systemModule;
        Vulkan* vulkan;
        VulkanPhysicalDevice* vulkanPhysicalDevice;
        VulkanDevice* vulkanDevice;
        IndexBuffer* indexBuffer;
        VertexBuffer* vertexBuffer;
        Shader* shader;
        Texture* whiteTexture;
        Renderer *renderer;

    public:
        explicit GraphicsModule(SystemModule* systemModule);

        ~GraphicsModule();

        Renderer *getRenderer() const;

        bool initialize(const Config& config) const;

        void terminate() const;
    };
}
