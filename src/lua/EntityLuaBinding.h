#pragma once

#include <entt/entt.hpp>
#include <lua.hpp>

namespace Blink {
    class EntityLuaBinding {
    private:
        entt::registry* entityRegistry;

    private:
        EntityLuaBinding(entt::registry* entityRegistry);

        ~EntityLuaBinding() = default;

    public:
        static void initialize(lua_State* L, entt::registry* entityRegistry);

    private:
        static int destroy(lua_State* L);

        static int index(lua_State* L);

        static int getPosition(lua_State* L);

        static int setPosition(lua_State* L);
    };
}
