#pragma once

#include "AppConfig.h"
#include "system/FileSystem.h"
#include "window/Window.h"
#include "window/Keyboard.h"
#include "window/Mouse.h"
#include "graphics/MeshManager.h"
#include "graphics/Renderer.h"
#include "graphics/ShaderManager.h"
#include "graphics/VulkanApp.h"
#include "graphics/VulkanPhysicalDevice.h"
#include "graphics/VulkanDevice.h"
#include "lua/LuaEngine.h"
#include "scene/SceneCamera.h"
#include "scene/Scene.h"
#include "window/Event.h"

namespace Blink {
    enum class AppState {
        None = 0,
        Initialized = 1,
        Running = 2,
        Paused = 3,
    };

    class App {
    private:
        AppConfig config;
        AppState state = AppState::None;
        FileSystem* fileSystem = nullptr;
        Window* window = nullptr;
        Keyboard* keyboard = nullptr;
        Mouse* mouse = nullptr;
        VulkanApp* vulkanApp = nullptr;
        VulkanPhysicalDevice* vulkanPhysicalDevice = nullptr;
        VulkanDevice* vulkanDevice = nullptr;
        MeshManager* meshManager = nullptr;
        ShaderManager* shaderManager = nullptr;
        Renderer* renderer = nullptr;
        LuaEngine* luaEngine = nullptr;
        SceneCamera* sceneCamera = nullptr;
        Scene* scene = nullptr;
        double lastTime = 0.0;
        double secondsSinceLastStatsUpdate = 0.0;
        uint32_t fps = 0;
        uint32_t ups = 0;

        double secondsSinceLastFrame = 0.0;
        double lastUpdateTime = 0.0;
        double secondsPerFrame = 1.0 / 60.0;

    public:
        explicit App(const AppConfig& config);

        ~App();

        void run();

    private:
        void update();

        void onEvent(Event& event);

        void initialize();

        void terminate() const;
    };
}
