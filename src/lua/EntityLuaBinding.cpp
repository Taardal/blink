#include "pch.h"
#include "EntityLuaBinding.h"
#include "printLua.h"
#include "game/Components.h"

namespace Blink {
    void EntityLuaBinding::sayHello() {
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
    }

    int EntityLuaBinding::sayHello1(lua_State* L) {
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -1);
        binding->sayHello();
        return 0;
    }

    EntityLuaBinding::EntityLuaBinding(entt::registry* registry): registry(registry) {
    }

    void EntityLuaBinding::create(lua_State* L, entt::registry* entityRegistry) {
        std::string typeName = "Entity";
        std::string metatableName = typeName + "__meta";

        // Allocate memory for the C++ object and push a userdata onto the Lua stack
        void* userdata = lua_newuserdata(L, sizeof(EntityLuaBinding));

        // Construct the C++ object in the allocated memory block
        new(userdata) EntityLuaBinding(entityRegistry);

        // Lua stack
        // - [-1] userdata  {EntityBinding}

        // Create a new metatable and push it onto the Lua stack
        luaL_newmetatable(L, metatableName.c_str());

        // Lua stack
        // - [-1] table     {metatable}
        // - [-2] userdata  {EntityBinding}

        // Set the __gc metamethod of the metatable to EntityBinding::destroy
        {
            lua_pushstring(L, "__gc");

            // Lua stack
            // - [-1] string    {"__gc"}
            // - [-2] table     {metatable}
            // - [-3] userdata  {EntityBinding}

            lua_pushcfunction(L, EntityLuaBinding::destroy);

            // Lua stack
            // - [-1] function  {EntityBinding::destroy}
            // - [-2] string    {"__gc"}
            // - [-3] table     {metatable}
            // - [-4] userdata  {EntityBinding}

            lua_settable(L, -3);
        }

        // Lua stack
        // - [-1] table     {metatable}
        // - [-2] userdata  {EntityBinding}

        // Set the __index metamethod of the metatable to EntityBinding::index
        {
            lua_pushstring(L, "__index");

            // Lua stack
            // - [-1] string    {"__index"}
            // - [-2] table     {metatable}
            // - [-3] userdata  {EntityBinding}

            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, EntityLuaBinding::index, upvalueCount);

            // Lua stack
            // - [-1] function  {EntityBinding::index}
            // - [-2] string    {"__index"}
            // - [-3] table     {metatable}
            // - [-4] userdata  {EntityBinding}

            lua_settable(L, -3);
        }

        // Lua stack
        // - [-1] table     {metatable}
        // - [-2] userdata  {EntityBinding}

        // Set the newly created metatable as the metatable of the userdata
        lua_setmetatable(L, -2);

        // Lua stack
        // - [-1] userdata  {EntityBinding}

        lua_newtable(L); // Create Lua object

        // Lua stack
        // - [-1] table     {typeName}
        // - [-2] userdata  {EntityBinding}

        // Associate the Lua table with the userdata (C++ object) as its user value
        lua_setuservalue(L, -2);

        // Lua stack
        // - [-1] userdata  {EntityBinding}

        lua_pushlightuserdata(L, entityRegistry);

        // Lua stack
        // - [-1] userdata  {entt::registry*}
        // - [-2] userdata  {EntityBinding}

        // Associate the entity registry to the userdata as a light user value (C++ pointer)
        lua_setuservalue(L, -2);

        // Create a global Lua variable and assign the userdata to it (C++ object)
        lua_setglobal(L, typeName.c_str());
    }

    int EntityLuaBinding::destroy(lua_State* L) {
        // Lua stack
        // - [-1] userdata  {EntityBinding}
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -1);
        binding->~EntityLuaBinding();
        return 0;
    }


    int EntityLuaBinding::index(lua_State* L) {
        // Lua stack
        // - [-1] string    {index name}
        // - [-2] userdata  {EntityBinding}

        std::string key = lua_tostring(L, -1);
        BL_LOG_DEBUG("Key [{}]", key.c_str());

        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);

        if (key == "sayHello") {
            lua_pushcfunction(L, binding->sayHello1);
            return 1;
        }
        if (key == "getUUID") {
            lua_pushcfunction(L, [](lua_State* L) -> int {
                // Lua stack
                // - [-1] number    {entityId}
                // - [-2] userdata  {EntityBinding}
                entt::entity entity = (entt::entity) lua_tonumber(L, -1);
                auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
                auto& idComponent = binding->registry->get<IdComponent>(entity);
                lua_pushstring(L, idComponent.id.c_str());
                return 1;
            });
            return 1;
        }
        if (key == "getPosition") {
            lua_pushcfunction(L, [](lua_State* L) -> int {
                // Lua stack
                // - [-1] number    {entityId}
                // - [-2] userdata  {EntityBinding}
                entt::entity entity = (entt::entity) lua_tonumber(L, -1);
                auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
                auto& transformComponent = binding->registry->get<TransformComponent>(entity);
                const glm::vec3& position = transformComponent.position;
                lua_newtable(L);
                lua_pushnumber(L, position.x);
                lua_setfield(L, -2, "x");
                lua_pushnumber(L, position.y);
                lua_setfield(L, -2, "y");
                lua_pushnumber(L, position.z);
                lua_setfield(L, -2, "z");
                return 1;
            });
            return 1;
        }
        return 0;
    }

}
