#pragma once

#include "AppConfig.h"
#include "window/Window.h"
#include "graphics/Renderer.h"
#include "game/Camera.h"
#include "game/Scene.h"

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
        App(const AppConfig& appConfig);

        ~App();

        void run() const;
    };
}
