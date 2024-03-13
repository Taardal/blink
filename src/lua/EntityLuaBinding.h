#pragma once

#include <entt/entt.hpp>
#include <lua/lua.hpp>

namespace Blink {
    class EntityLuaBinding {
    private:
        entt::registry* registry;

    private:
        EntityLuaBinding(entt::registry* registry);

        ~EntityLuaBinding() = default;

    public:
        static void create(lua_State* L, entt::registry* entityRegistry);

        static void sayHello();

        static int sayHello1(lua_State* L);

    private:
        static int destroy(lua_State* L);

        static int index(lua_State* L);
    };
}
