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
    struct AppContext {
        FileSystem* fileSystem;
        Window* window;
        Keyboard* keyboard;
        Vulkan* vulkan;
        VulkanPhysicalDevice* physicalDevice;
        VulkanDevice* device;
        VulkanSwapChain* swapChain;
        VulkanRenderPass* renderPass;
        VulkanCommandPool* commandPool;
        Renderer* renderer;
        LuaEngine* luaEngine;
        Camera* camera;
        Scene* scene;
        App* app;

        explicit AppContext(const AppConfig& appConfig);

        ~AppContext();
    };
}
