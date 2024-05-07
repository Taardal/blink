#include "pch.h"
#include "EntityLuaBinding.h"
#include "luaUtils.h"
#include "scene/Components.h"

namespace Blink {
    EntityLuaBinding::EntityLuaBinding(entt::registry* entityRegistry): entityRegistry(entityRegistry) {
    }

    void EntityLuaBinding::initialize(lua_State* L, entt::registry* entityRegistry) {
        std::string typeName = "Entity";
        std::string metatableName = typeName + "__meta";

        // Allocate memory for the C++ object and push a userdata onto the Lua stack
        void* userdata = lua_newuserdata(L, sizeof(EntityLuaBinding));

        // Construct the C++ object in the allocated memory block
        new(userdata) EntityLuaBinding(entityRegistry);

        // Create a new metatable and push it onto the Lua stack
        luaL_newmetatable(L, metatableName.c_str());

        // Set the __gc metamethod of the metatable to EntityBinding::destroy
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, EntityLuaBinding::destroy);
        lua_settable(L, -3);

        // Set the __index metamethod of the metatable to EntityBinding::index
        lua_pushstring(L, "__index");
        constexpr int upvalueCount = 0;
        lua_pushcclosure(L, EntityLuaBinding::index, upvalueCount);
        lua_settable(L, -3);

        // Set the newly created metatable as the metatable of the userdata
        lua_setmetatable(L, -2);

        // Create Lua object to represent the userdata (C++ object) in Lua code
        lua_newtable(L);

        // Associate the Lua object with the userdata (C++ object) by assigning the Lua object as the userdata's user value
        lua_setuservalue(L, -2);

        // Create a global Lua variable and associate the userdata (C++ object) with it
        lua_setglobal(L, typeName.c_str());
    }

    // Lua stack
    // - [-1] userdata  EntityBinding
    int EntityLuaBinding::destroy(lua_State* L) {
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -1);
        binding->~EntityLuaBinding();
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] userdata  EntityBinding
    int EntityLuaBinding::index(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "getPosition") {
            lua_pushcfunction(L, EntityLuaBinding::getPosition);
            return 1;
        }
        if (indexName == "setPosition") {
            lua_pushcfunction(L, EntityLuaBinding::setPosition);
            return 1;
        }
        if (indexName == "getIdByTag") {
            lua_pushcfunction(L, EntityLuaBinding::getIdByTag);
            return 1;
        }
        if (indexName == "getTransformComponent") {
            lua_pushcfunction(L, EntityLuaBinding::getTransformComponent);
            return 1;
        }
        if (indexName == "setTransformComponent") {
            lua_pushcfunction(L, EntityLuaBinding::setTransformComponent);
            return 1;
        }
        BL_LOG_WARN("Could not index [{}]", indexName);
        return 0;
    }

    // Lua stack
    // - [-1] number    Entity ID
    // - [-2] userdata  EntityBinding
    int EntityLuaBinding::getPosition(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -1);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
        auto& transformComponent = binding->entityRegistry->get<TransformComponent>(entity);
        const glm::vec3& position = transformComponent.position;
        lua_newtable(L);
        lua_pushnumber(L, position.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, position.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, position.z);
        lua_setfield(L, -2, "z");
        return 1;
    }

    // Lua stack
    // - [-1] table    Position vector
    // - [-2] number   Entity ID
    // - [-3] userdata EntityBinding
    int EntityLuaBinding::setPosition(lua_State* L) {
        lua_getfield(L, -1, "x");
        auto x = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "y");
        auto y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "z");
        auto z = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        entt::entity entity = (entt::entity) lua_tonumber(L, -2);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -3);
        auto& transformComponent = binding->entityRegistry->get<TransformComponent>(entity);
        transformComponent.position = { x, y, z };

        return 0;
    }

    // Lua stack
    // - [-1] string   Entity tag
    // - [-2] userdata EntityBinding
    int EntityLuaBinding::getIdByTag(lua_State* L) {
        const char* entityTag = lua_tostring(L, -1);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
        for (entt::entity entity : binding->entityRegistry->view<TagComponent>()) {
            auto& [tag] = binding->entityRegistry->get<TagComponent>(entity);
            if (tag == entityTag) {
                lua_pushnumber(L, (uint32_t) entity);
                return 1;
            }
        }
        BL_LOG_WARN("Could not find entity by tag [{}]", entityTag);
        return 0;
    }

    // Lua stack
    // - [-1] number    Entity ID
    // - [-2] userdata  EntityBinding
    int EntityLuaBinding::getTransformComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -1);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
        auto& transformComponent = binding->entityRegistry->get<TransformComponent>(entity);

        lua_newtable(L);

        const glm::vec3& position = transformComponent.position;
        lua_newtable(L);
        lua_pushnumber(L, position.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, position.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, position.z);
        lua_setfield(L, -2, "z");
        lua_setfield(L, -2, "position");

        const glm::vec3& forwardDirection = transformComponent.forwardDirection;
        lua_newtable(L);
        lua_pushnumber(L, forwardDirection.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, forwardDirection.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, forwardDirection.z);
        lua_setfield(L, -2, "z");
        lua_setfield(L, -2, "forwardDirection");

        const glm::vec3& rightDirection = transformComponent.rightDirection;
        lua_newtable(L);
        lua_pushnumber(L, rightDirection.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, rightDirection.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, rightDirection.z);
        lua_setfield(L, -2, "z");
        lua_setfield(L, -2, "rightDirection");

        const glm::vec3& upDirection = transformComponent.upDirection;
        lua_newtable(L);
        lua_pushnumber(L, upDirection.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, upDirection.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, upDirection.z);
        lua_setfield(L, -2, "z");
        lua_setfield(L, -2, "upDirection");

        const glm::vec3& worldUpDirection = transformComponent.worldUpDirection;
        lua_newtable(L);
        lua_pushnumber(L, worldUpDirection.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, worldUpDirection.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, worldUpDirection.z);
        lua_setfield(L, -2, "z");
        lua_setfield(L, -2, "worldUpDirection");

        const float yaw = transformComponent.yaw;
        lua_pushnumber(L, yaw);
        lua_setfield(L, -2, "yaw");

        const float pitch = transformComponent.pitch;
        lua_pushnumber(L, pitch);
        lua_setfield(L, -2, "pitch");

        const float roll = transformComponent.roll;
        lua_pushnumber(L, roll);
        lua_setfield(L, -2, "roll");

        return 1;
    }

    // Lua stack
    // - [-1] table    Transform component table
    // - [-2] number   Entity ID
    // - [-3] userdata EntityBinding
    int EntityLuaBinding::setTransformComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -2);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -3);
        auto& transformComponent = binding->entityRegistry->get<TransformComponent>(entity);
        {
            lua_getfield(L, -1, "position");
            lua_getfield(L, -1, "x");
            auto x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "y");
            auto y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "z");
            auto z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            transformComponent.position = { x, y, z };
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "forwardDirection");
            lua_getfield(L, -1, "x");
            auto x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "y");
            auto y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "z");
            auto z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            transformComponent.forwardDirection = { x, y, z };
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "rightDirection");
            lua_getfield(L, -1, "x");
            auto x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "y");
            auto y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "z");
            auto z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            transformComponent.rightDirection = { x, y, z };
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "upDirection");
            lua_getfield(L, -1, "x");
            auto x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "y");
            auto y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "z");
            auto z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            transformComponent.upDirection = { x, y, z };
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "worldUpDirection");
            lua_getfield(L, -1, "x");
            auto x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "y");
            auto y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "z");
            auto z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            transformComponent.worldUpDirection = { x, y, z };
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "yaw");
            auto yaw = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            transformComponent.yaw = yaw;
        }
        {
            lua_getfield(L, -1, "pitch");
            auto pitch = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            transformComponent.pitch = pitch;
        }
        {
            lua_getfield(L, -1, "roll");
            auto roll = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
            transformComponent.roll = roll;
        }
        return 0;
    }
}
