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

        bool initialize();

        void terminate() const;

        glm::mat4 update(double timestep);

    private:
        entt::entity createEntity();

        void destroyEntity(entt::entity entity);
    };
}
