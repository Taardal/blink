#pragma once

#include "window/Keyboard.h"
#include "graphics/Renderer.h"
#include "lua/LuaEngine.h"
#include "scene/SceneCamera.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace Blink {
    struct SceneConfig {
        std::string name;
        Keyboard* keyboard = nullptr;
        MeshManager* meshManager = nullptr;
        Renderer* renderer = nullptr;
        LuaEngine* luaEngine = nullptr;
        SceneCamera* sceneCamera = nullptr;
    };

    class Scene {
    private:
        SceneConfig config;
        entt::registry registry;
        bool useSceneCamera = false;

    public:
        explicit Scene(const SceneConfig& config);

        ~Scene();

        void onEvent(Event& event);

        void update(double timestep);

        void render();

    private:
        void initializeScene();

        void initializeVikingRoomScene();

        void initializeFighterJetScene();
    };
}
