#pragma once

#include "EntityLuaBinding.h"
#include "window/Keyboard.h"

#include <lua/lua.hpp>

namespace Blink {
    class LuaEngine {
    private:
        lua_State* L;
        Keyboard* keyboard;

    public:
        explicit LuaEngine(Keyboard* keyboard);

        bool initialize() const;

        void terminate() const;

        void createEntityBindings(entt::registry* entityRegistry) const;

        void updateEntityBindings(entt::registry* entityRegistry, double timestep) const;

    private:
        void createGlobalBindings() const;

        static int luaPrint(lua_State* L);
    };
}
