#pragma once

#include "EntityLuaBinding.h"
#include "window/Keyboard.h"

#include <lua.hpp>

namespace Blink {
    struct LuaEngineConfig {
        Keyboard* keyboard;
    };

    class LuaEngine {
    private:
        LuaEngineConfig config;
        lua_State* L;

    public:
        explicit LuaEngine(const LuaEngineConfig& config);

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
