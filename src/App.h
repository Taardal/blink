#pragma once

#include "system/FileSystem.h"
#include "window/Window.h"
#include "window/Keyboard.h"
#include "window/Mouse.h"
#include "graphics/MeshManager.h"
#include "graphics/Renderer.h"
#include "graphics/ShaderManager.h"
#include "graphics/SkyboxManager.h"
#include "graphics/VulkanApp.h"
#include "graphics/VulkanPhysicalDevice.h"
#include "graphics/VulkanDevice.h"
#include "lua/LuaEngine.h"
#include "scene/SceneCamera.h"
#include "scene/Scene.h"
#include "window/Event.h"

namespace Blink {
    struct AppConfig {
        std::string name = "App";
        std::vector<std::string> scenes;
        int32_t windowWidth = 800;
        int32_t windowHeight = 600;
        bool windowMaximized = false;
        bool windowResizable = false;
    };

    class App {
    private:
        AppConfig config;
        bool initialized = false;
        bool running = false;
        bool paused = false;
        FileSystem* fileSystem = nullptr;
        Window* window = nullptr;
        Keyboard* keyboard = nullptr;
        Mouse* mouse = nullptr;
        VulkanApp* vulkanApp = nullptr;
        VulkanPhysicalDevice* vulkanPhysicalDevice = nullptr;
        VulkanDevice* vulkanDevice = nullptr;
        MeshManager* meshManager = nullptr;
        ShaderManager* shaderManager = nullptr;
        SkyboxManager* skyboxManager = nullptr;
        Renderer* renderer = nullptr;
        LuaEngine* luaEngine = nullptr;
        SceneCamera* sceneCamera = nullptr;
        Scene* scene = nullptr;

    public:
        explicit App(const AppConfig& config);

        ~App();

        void run();

    private:
        void gameLoop() const;

        void onEvent(Event& event);

        void setScene(const std::string& scenePath);

        void initialize();

        void terminate() const;
    };
}
