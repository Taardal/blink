#pragma once

#include "graphics/Vulkan.h"
#include "graphics/VulkanPhysicalDevice.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanCommandPool.h"
#include "graphics/Renderer.h"
#include "system/SystemModule.h"
#include "window/WindowModule.h"

namespace Blink {
    class GraphicsModule {
    private:
        SystemModule* systemModule;
        Vulkan* vulkan;
        VulkanPhysicalDevice* physicalDevice;
        VulkanDevice* device;
        VulkanSwapChain* swapChain;
        VulkanRenderPass* renderPass;
        VulkanCommandPool* commandPool;
        Renderer *renderer;

    public:
        GraphicsModule(SystemModule* systemModule, WindowModule* windowModule);

        ~GraphicsModule();

        Renderer *getRenderer() const;

        bool initialize(const AppConfig& appConfig);

        void terminate() const;
    };
}
