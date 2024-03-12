#include "pch.h"
#include "EntityBinding.h"
#include "printLua.h"
#include "game/Components.h"

namespace Blink {
    void EntityBinding::sayHello() {
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
        printf("HELLO YES IT IS I\n");
    }

    EntityBinding::EntityBinding(entt::registry* registry): registry(registry) {
    }

    void EntityBinding::create(lua_State* L, entt::registry* entityRegistry) {
        std::string typeName = "Entity";
        std::string metatableName = typeName + "__meta";

        // Allocate memory for the C++ object and push a userdata onto the Lua stack
        void* userdata = lua_newuserdata(L, sizeof(EntityBinding));

        // Construct the C++ object in the allocated memory block
        new(userdata) EntityBinding(entityRegistry);

        // - [-1] userdata  {EntityBinding}

        // Create a new metatable and push it onto the Lua stack
        luaL_newmetatable(L, metatableName.c_str());

        // - [-1] table     {metatable}
        // - [-2] userdata  {EntityBinding}

        // Set the __gc metamethod of the metatable to EntityBinding::destroy
        {
            lua_pushstring(L, "__gc");

            // - [-1] string    {"__gc"}
            // - [-2] table     {metatable}
            // - [-3] userdata  {EntityBinding}

            lua_pushcfunction(L, EntityBinding::destroy);

            // - [-1] function  {EntityBinding::destroy}
            // - [-2] string    {"__gc"}
            // - [-3] table     {metatable}
            // - [-4] userdata  {EntityBinding}

            lua_settable(L, -3);
        }

        // - [-1] table     {metatable}
        // - [-2] userdata  {EntityBinding}

        // Set the __index metamethod of the metatable to EntityBinding::index
        {
            lua_pushstring(L, "__index");

            // - [-1] string    {"__index"}
            // - [-2] table     {metatable}
            // - [-3] userdata  {EntityBinding}

            constexpr int upvalueCount = 0;
            lua_pushcclosure(L, EntityBinding::index, upvalueCount);

            // - [-1] function  {EntityBinding::index}
            // - [-2] string    {"__index"}
            // - [-3] table     {metatable}
            // - [-4] userdata  {EntityBinding}

            lua_settable(L, -3);
        }

        // - [-1] table     {metatable}
        // - [-2] userdata  {EntityBinding}

        // Set the newly created metatable as the metatable of the userdata
        lua_setmetatable(L, -2);

        // - [-1] userdata  {EntityBinding}

        lua_newtable(L); // Create Lua object

        // - [-1] table     {typeName}
        // - [-2] userdata  {EntityBinding}

        // Associate the Lua table with the userdata (C++ object) as its user value
        lua_setuservalue(L, -2);

        // - [-1] userdata  {EntityBinding}

        // Create a global Lua variable and assign the userdata to it (C++ object)
        lua_setglobal(L, typeName.c_str());
    }

    int EntityBinding::destroy(lua_State* L) {
        // - [-1] userdata  {EntityBinding}
        auto* binding = (EntityBinding*) lua_touserdata(L, -1);
        binding->~EntityBinding();
        return 0;
    }


    int EntityBinding::index(lua_State* L) {
        BL_LOG_INFO("NOTICE ME [INDEX]");

        // - [-1] string    {index name}
        // - [-2] userdata  {EntityBinding}

        std::string key = lua_tostring(L, -1);
        printf("key [%s]\n", key.c_str());

        if (key == "sayHello") {
            lua_pushcfunction(L, [](lua_State* L) -> int {
                printLua(L, "sayHeyllo");
                auto* binding = (EntityBinding*) lua_touserdata(L, -1);
                binding->sayHello();
                return 0;
            });
            return 1;
        }
        if (key == "sayFoo") {
            lua_pushcfunction(L, [](lua_State* L) -> int {
                lua_pushstring(L, "foo");
                return 1;
            });
            return 1;
        }
        if (key == "getUUID") {
            lua_pushcfunction(L, [](lua_State* L) -> int {
                printLua(L, "getUUID");
                entt::entity entity = (entt::entity) lua_tonumber(L, -1);
                auto* binding = (EntityBinding*) lua_touserdata(L, -2);
                auto& idComponent = binding->registry->get<IdComponent>(entity);
                lua_pushstring(L, idComponent.id.c_str());
                return 1;
            });
            return 1;
        }
        return 0;
    }

}
