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

        void onEvent(Event& event);

        glm::mat4 update(double timestep);

    private:
        void initializeEntityComponents();
    };
}
