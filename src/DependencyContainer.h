#pragma once

#include "system/FileSystem.h"
#include "window/Keyboard.h"
#include "window/Window.h"
#include "graphics/Vulkan.h"
#include "graphics/VulkanCommandPool.h"
#include "graphics/VulkanDevice.h"
#include "graphics/VulkanPhysicalDevice.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/Renderer.h"
#include "lua/LuaEngine.h"
#include "game/Camera.h"
#include "game/Scene.h"
#include "game/Game.h"
#include "App.h"

namespace Blink {
    struct DependencyContainer {
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
        App* app = nullptr;

        explicit DependencyContainer(const AppConfig& appConfig);

        ~DependencyContainer();
    };
}
