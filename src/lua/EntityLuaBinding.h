#pragma once

#include "LuaBinding.h"

#include <entt/entt.hpp>
#include <lua/lua.hpp>

namespace Blink {
    struct EntityLuaBindingConfig {
        lua_State* L;
        std::string tableName;
        entt::registry* registry;
        entt::entity entity;
    };

    class EntityLuaBinding : public LuaBinding {
    private:
        entt::registry* registry;
        entt::entity entity;

    public:
        explicit EntityLuaBinding(const EntityLuaBindingConfig& config);

        ~EntityLuaBinding() = default;

        static void createType(lua_State* L, const std::string& typeName, entt::registry* registry);

        static int createInstance(lua_State* L);

        static int destroy(lua_State* L);

        static int index(lua_State* L);

        static int newIndex(lua_State* L);
    };
}
