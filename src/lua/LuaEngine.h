#pragma once

#include "EntityLuaBinding.h"
#include "window/Keyboard.h"

#include <lua.hpp>

namespace Blink {
    class LuaEngine {
    private:
        lua_State* L;
        Keyboard* keyboard;

    public:
        explicit LuaEngine(Keyboard* keyboard);

        ~LuaEngine();

        void reload(entt::registry* entityRegistry) const;

        void createEntityBindings(entt::registry* entityRegistry) const;

        void updateEntityBindings(entt::registry* entityRegistry, double timestep) const;

    private:
        void createGlobalBindings() const;

        static void compileLuaFiles();

        static int luaPrint(lua_State* L);
    };
}
