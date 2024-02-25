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
    struct GraphicsModule {
    private:
        Vulkan* vulkan;
        VulkanPhysicalDevice* physicalDevice;
        VulkanDevice* device;
        VulkanSwapChain* swapChain;
        VulkanRenderPass* renderPass;
        VulkanCommandPool* commandPool;

    public:
        Renderer* renderer;

    public:
        GraphicsModule(SystemModule* systemModule, WindowModule* windowModule);

        ~GraphicsModule();

        bool initialize(const AppConfig& appConfig);

        void terminate() const;
    };
}
