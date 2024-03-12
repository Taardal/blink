#pragma once

#include <entt/entt.hpp>
#include <lua/lua.hpp>

namespace Blink {
    class EntityBinding {
    private:
        entt::registry* registry;

    private:
        EntityBinding(entt::registry* registry);

        ~EntityBinding() = default;

    public:
        static void create(lua_State* L, entt::registry* entityRegistry);

        static void sayHello();

    private:
        static int destroy(lua_State* L);

        static int index(lua_State* L);
    };
}
