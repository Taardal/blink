#pragma once

#include "AppConfig.h"
#include "system/FileSystem.h"
#include "window/Window.h"
#include "window/Keyboard.h"
#include "graphics/Vulkan.h"
#include "graphics/VulkanPhysicalDevice.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanCommandPool.h"
#include "graphics/Renderer.h"
#include "lua/LuaEngine.h"
#include "scene/Camera.h"
#include "scene/Scene.h"
#include "window/Event.h"

namespace Blink {
    void runApp(const AppConfig& config);
}

namespace Blink {
    class App {
    private:
        FileSystem* fileSystem = nullptr;
        Window* window = nullptr;
        Keyboard* keyboard = nullptr;
        Vulkan* vulkan = nullptr;
        VulkanPhysicalDevice* physicalDevice = nullptr;
        VulkanDevice* device = nullptr;
        VulkanSwapChain* swapChain = nullptr;
        VulkanRenderPass* renderPass = nullptr;
        VulkanCommandPool* commandPool = nullptr;
        Renderer* renderer = nullptr;
        LuaEngine* luaEngine = nullptr;
        Camera* camera = nullptr;
        Scene* scene = nullptr;

    public:
        explicit App(const AppConfig& appConfig);

        ~App();

        void run() const;

    private:
        void onEvent(Event& event) const;
    };
}
