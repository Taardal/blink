#include "pch.h"
#include "lua/EntityLuaBinding.h"
#include "lua/GlmLuaBinding.h"
#include "lua/luaUtils.h"
#include "graphics/MeshManager.h"
#include "scene/Components.h"
#include "scene/Scene.h"

namespace Blink {
    EntityLuaBinding::EntityLuaBinding(Scene* scene) : scene(scene) {
    }

    void EntityLuaBinding::initialize(lua_State* L, Scene* scene) {
        std::string typeName = "Entity";
        std::string metatableName = typeName + "__meta";

        // Allocate memory for the C++ object and push a userdata onto the Lua stack
        void* userdata = lua_newuserdata(L, sizeof(EntityLuaBinding));

        // Construct the C++ object in the allocated memory block
        new(userdata) EntityLuaBinding(scene);

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
    // - [-2] userdata  Entity binding
    int EntityLuaBinding::index(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "create") {
            lua_pushcfunction(L, EntityLuaBinding::createEntity);
            return 1;
        }
        if (indexName == "setMeshComponent") {
            lua_pushcfunction(L, EntityLuaBinding::setMeshComponent);
            return 1;
        }
        if (indexName == "setLuaComponent") {
            lua_pushcfunction(L, EntityLuaBinding::setLuaComponent);
            return 1;
        }
        if (indexName == "getTagComponent") {
            lua_pushcfunction(L, EntityLuaBinding::getTagComponent);
            return 1;
        }
        if (indexName == "setTagComponent") {
            lua_pushcfunction(L, EntityLuaBinding::setTagComponent);
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
        if (indexName == "setCameraComponent") {
            lua_pushcfunction(L, EntityLuaBinding::setCameraComponent);
            return 1;
        }
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
        BL_LOG_WARN("Could not resolve index [{}]", indexName);
        return 0;
    }

    // Lua stack
    // - [-1] userdata  Entity binding
    int EntityLuaBinding::createEntity(lua_State* L) {
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -1);
        entt::entity entity = binding->scene->createEntity();
        lua_pushnumber(L, (uint32_t) entity);
        return 1;
    }

    // Lua stack
    // - [-1] table     Mesh component
    // - [-2] number    Entity ID
    // - [-3] userdata  Entity binding
    int EntityLuaBinding::setMeshComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -2);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -3);
        auto& meshComponent = binding->scene->entityRegistry.get_or_emplace<MeshComponent>(entity);

        MeshInfo meshInfo{};
        {
            lua_getfield(L, -1, "modelPath");
            meshInfo.modelPath = lua_tostring(L, -1);
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "textureAtlasPath");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                const char* textureAtlasPath = lua_tostring(L, -1);
                meshInfo.textureAtlasPath = textureAtlasPath;
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "texturesDirectoryPath");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                const char* texturesDirectoryPath = lua_tostring(L, -1);
                meshInfo.texturesDirectoryPath = texturesDirectoryPath;
            }
            lua_pop(L, 1);
        }

        // Only set the mesh metadata here
        // Resource loading should happen explicitly in the Scene class, after all components are created
        meshComponent.meshInfo = meshInfo;
        //meshComponent.mesh = binding->scene->config.meshManager->getMesh(meshInfo);

        return 0;
    }

    // Lua stack
    // - [-1] table     Lua component
    // - [-2] number    Entity ID
    // - [-3] userdata  Entity binding
    int EntityLuaBinding::setLuaComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -2);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -3);
        auto& luaComponent = binding->scene->entityRegistry.get_or_emplace<LuaComponent>(entity);

        lua_getfield(L, -1, "type");
        luaComponent.type = lua_tostring(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "path");
        luaComponent.path = lua_tostring(L, -1);
        lua_pop(L, 1);

        return 0;
    }

    // Lua stack
    // - [-1] number    Entity ID
    // - [-2] userdata  Entity binding
    int EntityLuaBinding::getTagComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -1);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
        auto& tagComponent = binding->scene->entityRegistry.get<TagComponent>(entity);

        lua_newtable(L);
        lua_pushstring(L, tagComponent.tag.c_str());
        lua_setfield(L, -2, "tag");

        return 1;
    }

    // Lua stack
    // - [-1] table     Tag component
    // - [-2] number    Entity ID
    // - [-3] userdata  Entity binding
    int EntityLuaBinding::setTagComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -2);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -3);
        auto& tagComponent = binding->scene->entityRegistry.get_or_emplace<TagComponent>(entity);

        lua_getfield(L, -1, "tag");
        tagComponent.tag = lua_tostring(L, -1);
        lua_pop(L, 1);

        return 0;
    }

    // Lua stack
    // - [-1] number    Entity ID
    // - [-2] userdata  Entity binding
    int EntityLuaBinding::getTransformComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -1);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
        auto& transformComponent = binding->scene->entityRegistry.get<TransformComponent>(entity);

        lua_newtable(L);

        const glm::vec3& position = transformComponent.position;
        lua_newtable(L);
        lua_pushnumber(L, position.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, position.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, position.z);
        lua_setfield(L, -2, "z");
        luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);
        lua_setfield(L, -2, "position");

        const glm::vec3& size = transformComponent.size;
        lua_newtable(L);
        lua_pushnumber(L, size.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, size.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, size.z);
        lua_setfield(L, -2, "z");
        luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);
        lua_setfield(L, -2, "size");

        const glm::vec3& forwardDirection = transformComponent.forwardDirection;
        lua_newtable(L);
        lua_pushnumber(L, forwardDirection.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, forwardDirection.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, forwardDirection.z);
        lua_setfield(L, -2, "z");
        luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);
        lua_setfield(L, -2, "forwardDirection");

        const glm::vec3& rightDirection = transformComponent.rightDirection;
        lua_newtable(L);
        lua_pushnumber(L, rightDirection.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, rightDirection.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, rightDirection.z);
        lua_setfield(L, -2, "z");
        luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);
        lua_setfield(L, -2, "rightDirection");

        const glm::vec3& upDirection = transformComponent.upDirection;
        lua_newtable(L);
        lua_pushnumber(L, upDirection.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, upDirection.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, upDirection.z);
        lua_setfield(L, -2, "z");
        luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);
        lua_setfield(L, -2, "upDirection");

        const glm::vec3& worldUpDirection = transformComponent.worldUpDirection;
        lua_newtable(L);
        lua_pushnumber(L, worldUpDirection.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, worldUpDirection.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, worldUpDirection.z);
        lua_setfield(L, -2, "z");
        luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);
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

        const float yawOffset = transformComponent.yawOffset;
        lua_pushnumber(L, yawOffset);
        lua_setfield(L, -2, "yawOffset");

        const float pitchOffset = transformComponent.pitchOffset;
        lua_pushnumber(L, pitchOffset);
        lua_setfield(L, -2, "pitchOffset");

        const float rollOffset = transformComponent.rollOffset;
        lua_pushnumber(L, rollOffset);
        lua_setfield(L, -2, "rollOffset");

        return 1;
    }

    // Lua stack
    // - [-1] table    Transform component table
    // - [-2] number   Entity ID
    // - [-3] userdata EntityBinding
    int EntityLuaBinding::setTransformComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -2);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -3);
        auto& transformComponent = binding->scene->entityRegistry.get_or_emplace<TransformComponent>(entity);
        {
            lua_getfield(L, -1, "position");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
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
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "size");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                lua_getfield(L, -1, "x");
                auto x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                lua_getfield(L, -1, "y");
                auto y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                lua_getfield(L, -1, "z");
                auto z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                transformComponent.size = { x, y, z };
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "forwardDirection");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
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
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "rightDirection");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                lua_getfield(L, -1, "x");
                auto x = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                lua_getfield(L, -1, "y");
                auto y = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                lua_getfield(L, -1, "z");
                auto z = (float) lua_tonumber(L, -1);
                lua_pop(L, 1);
                transformComponent.rightDirection = {x, y, z};
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "upDirection");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
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
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "worldUpDirection");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
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
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "yaw");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.yaw = (float) lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "pitch");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.pitch = (float) lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "roll");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.roll = (float) lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "yawOffset");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.yawOffset = (float) lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "pitchOffset");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.pitchOffset = (float) lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "rollOffset");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.rollOffset = (float) lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        return 0;
    }

    // Lua stack
    // - [-1] table    Camera component
    // - [-2] number   Entity ID
    // - [-3] userdata EntityBinding
    int EntityLuaBinding::setCameraComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -2);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -3);
        binding->scene->entityRegistry.emplace<CameraComponent>(entity);
        return 0;
    }


    // Lua stack
    // - [-1] number    Entity ID
    // - [-2] userdata  Entity binding
    int EntityLuaBinding::getPosition(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -1);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
        auto& transformComponent = binding->scene->entityRegistry.get<TransformComponent>(entity);
        const glm::vec3& position = transformComponent.position;
        lua_newtable(L);
        lua_pushnumber(L, position.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, position.y);
        lua_setfield(L, -2, "y");
        lua_pushnumber(L, position.z);
        lua_setfield(L, -2, "z");
        luaL_getmetatable(L, GlmLuaBinding::VEC3_METATABLE_NAME.c_str());
        lua_setmetatable(L, -2);
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
        auto& transformComponent = binding->scene->entityRegistry.get<TransformComponent>(entity);
        transformComponent.position = { x, y, z };

        return 0;
    }

    // Lua stack
    // - [-1] string   Entity tag
    // - [-2] userdata EntityBinding
    int EntityLuaBinding::getIdByTag(lua_State* L) {
        const char* entityTag = lua_tostring(L, -1);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
        for (entt::entity entity : binding->scene->entityRegistry.view<TagComponent>()) {
            auto& [tag] = binding->scene->entityRegistry.get<TagComponent>(entity);
            if (tag == entityTag) {
                lua_pushnumber(L, (uint32_t) entity);
                return 1;
            }
        }
        BL_LOG_WARN("Could not find entity by tag [{}]", entityTag);
        return 0;
    }
}
