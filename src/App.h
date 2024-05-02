#pragma once

#include "AppConfig.h"
#include "system/FileSystem.h"
#include "window/Window.h"
#include "window/Keyboard.h"
#include "graphics/MeshManager.h"
#include "graphics/Renderer.h"
#include "graphics/ShaderManager.h"
#include "graphics/VulkanApp.h"
#include "graphics/VulkanPhysicalDevice.h"
#include "graphics/VulkanDevice.h"
#include "lua/LuaEngine.h"
#include "scene/Camera.h"
#include "scene/Scene.h"
#include "window/Event.h"

namespace Blink {
    class App {
    private:
        AppConfig config;
        FileSystem* fileSystem = nullptr;
        Window* window = nullptr;
        Keyboard* keyboard = nullptr;
        VulkanApp* vulkanApp = nullptr;
        VulkanPhysicalDevice* vulkanPhysicalDevice = nullptr;
        VulkanDevice* vulkanDevice = nullptr;
        MeshManager* meshManager = nullptr;
        ShaderManager* shaderManager = nullptr;
        Renderer* renderer = nullptr;
        LuaEngine* luaEngine = nullptr;
        Camera* camera = nullptr;
        Scene* scene = nullptr;
        bool initialized = false;
        double lastTime = 0.0;
        double fpsUpdateTimestep = 0.0;
        uint32_t fps = 0;

    public:
        explicit App(const AppConfig& config);

        ~App();

        void run();

    private:
        void update();

        void render() const;

        void onEvent(Event& event) const;

        void initialize();

        void terminate() const;
    };
}
