#pragma once

#include "window/Keyboard.h"
#include "graphics/Renderer.h"
#include "lua/LuaEngine.h"
#include "scene/Camera.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace Blink {
    struct SceneConfig {
        Keyboard* keyboard = nullptr;
        ResourceLoader* resourceLoader = nullptr;
        Renderer* renderer = nullptr;
        LuaEngine* luaEngine = nullptr;
        Camera* camera = nullptr;
    };

    class Scene {
    private:
        SceneConfig config;
        entt::registry registry;
        entt::entity player;
        entt::entity enemy;

    public:
        explicit Scene(const SceneConfig& config);

        ~Scene();

        void onEvent(Event& event);

        void update(double timestep);

        void render();

    private:
        void initializePlayerComponents();

        void initializeEnemyComponents();
    };
}
