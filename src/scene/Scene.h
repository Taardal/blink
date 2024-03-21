#pragma once

#include "window/Keyboard.h"
#include "lua/LuaEngine.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace Blink {
    class Scene {
    private:
        Keyboard* keyboard;
        LuaEngine* luaEngine;
        entt::registry registry;
        entt::entity player;

    public:
        Scene(Keyboard* keyboard, LuaEngine* luaEngine);

        ~Scene();

        glm::mat4 update(double timestep);

        void onEvent(Event& event);

    private:
        void initializeEntityComponents();
    };
}
