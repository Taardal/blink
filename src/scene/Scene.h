#pragma once

#include "window/Keyboard.h"
#include "lua/LuaEngine.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace Blink {
    struct SceneConfig {
        Keyboard* keyboard = nullptr;
        LuaEngine* luaEngine = nullptr;
    };

    class Scene {
    private:
        SceneConfig config;
        entt::registry registry;
        entt::entity player;

    public:
        explicit Scene(const SceneConfig& config);

        ~Scene();

        glm::mat4 update(double timestep);

        void onEvent(Event& event);

    private:
        void initializeEntityComponents();
    };
}
