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
    // - [-1] userdata  Binding
    int EntityLuaBinding::destroy(lua_State* L) {
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -1);
        binding->~EntityLuaBinding();
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] userdata  Binding
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
        if (indexName == "getCameraComponent") {
            lua_pushcfunction(L, EntityLuaBinding::getCameraComponent);
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
        if (indexName == "getEntityByTag") {
            lua_pushcfunction(L, EntityLuaBinding::getEntityByTag);
            return 1;
        }
        BL_LOG_WARN("Could not resolve index [{}]", indexName);
        return 0;
    }

    // Lua stack
    // - [-1] userdata  Binding
    int EntityLuaBinding::createEntity(lua_State* L) {
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -1);
        entt::entity entity = binding->scene->createEntityWithDefaultComponents();
        lua_pushnumber(L, (uint32_t) entity);
        return 1;
    }

    // Lua stack
    // - [-1] table     Mesh component
    // - [-2] number    Entity
    // - [-3] userdata  Binding
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
    // - [-2] number    Entity
    // - [-3] userdata  Binding
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
    // - [-1] number    Entity
    // - [-2] userdata  Binding
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
    // - [-2] number    Entity
    // - [-3] userdata  Binding
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
    // - [-1] number    Entity
    // - [-2] userdata  Binding
    int EntityLuaBinding::getTransformComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -1);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
        auto& transformComponent = binding->scene->entityRegistry.get<TransformComponent>(entity);

        lua_newtable(L);

        lua_pushmat4(L, transformComponent.translation);
        lua_setfield(L, -2, "translation");

        lua_pushmat4(L, transformComponent.rotation);
        lua_setfield(L, -2, "rotation");

        lua_pushmat4(L, transformComponent.scale);
        lua_setfield(L, -2, "scale");

        lua_pushvec3(L, transformComponent.position);
        lua_setfield(L, -2, "position");

        lua_pushvec3(L, transformComponent.size);
        lua_setfield(L, -2, "size");

        lua_pushvec3(L, transformComponent.forwardDirection);
        lua_setfield(L, -2, "forwardDirection");

        lua_pushvec3(L, transformComponent.rightDirection);
        lua_setfield(L, -2, "rightDirection");

        lua_pushvec3(L, transformComponent.upDirection);
        lua_setfield(L, -2, "upDirection");

        lua_pushvec3(L, transformComponent.worldUpDirection);
        lua_setfield(L, -2, "worldUpDirection");

        lua_pushquat(L, transformComponent.orientation);
        lua_setfield(L, -2, "orientation");

        lua_pushnumber(L, transformComponent.yaw);
        lua_setfield(L, -2, "yaw");

        lua_pushnumber(L, transformComponent.pitch);
        lua_setfield(L, -2, "pitch");

        lua_pushnumber(L, transformComponent.roll);
        lua_setfield(L, -2, "roll");

        return 1;
    }

    // Lua stack
    // - [-1] table    Transform component table
    // - [-2] number   Entity
    // - [-3] userdata Binding
    int EntityLuaBinding::setTransformComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -2);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -3);
        auto& transformComponent = binding->scene->entityRegistry.get_or_emplace<TransformComponent>(entity);
        {
            lua_getfield(L, -1, "translation");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.translation = lua_tomat4(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "rotation");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.rotation = lua_tomat4(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "scale");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.scale = lua_tomat4(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "position");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.position = lua_tovec3(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "size");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.size = lua_tovec3(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "forwardDirection");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.forwardDirection = lua_tovec3(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "rightDirection");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.rightDirection = lua_tovec3(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "upDirection");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.upDirection = lua_tovec3(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "worldUpDirection");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.worldUpDirection = lua_tovec3(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "orientation");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                transformComponent.orientation = lua_toquat(L, -1);
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
        return 0;
    }

    // Lua stack
    // - [-1] number    Entity
    // - [-2] userdata  Binding
    int EntityLuaBinding::getCameraComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -1);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
        auto& cameraComponent = binding->scene->entityRegistry.get<CameraComponent>(entity);

        lua_newtable(L);

        lua_pushnumber(L, cameraComponent.aspectRatio);
        lua_setfield(L, -2, "aspectRatio");

        lua_pushnumber(L, cameraComponent.fieldOfView);
        lua_setfield(L, -2, "fieldOfView");

        lua_pushnumber(L, cameraComponent.farClip);
        lua_setfield(L, -2, "farClip");

        lua_pushnumber(L, cameraComponent.nearClip);
        lua_setfield(L, -2, "nearClip");

        lua_pushmat4(L, cameraComponent.view);
        lua_setfield(L, -2, "view");

        return 1;
    }

    // Lua stack
    // - [-1] table    Camera component
    // - [-2] number   Entity
    // - [-3] userdata Binding
    int EntityLuaBinding::setCameraComponent(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -2);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -3);
        auto& cameraComponent = binding->scene->entityRegistry.get_or_emplace<CameraComponent>(entity);
        {
            lua_getfield(L, -1, "aspectRatio");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                cameraComponent.aspectRatio = (float) lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "fieldOfView");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                cameraComponent.fieldOfView = (float) lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "nearClip");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                cameraComponent.nearClip = (float) lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "farClip");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                cameraComponent.farClip = (float) lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
        {
            lua_getfield(L, -1, "view");
            bool missing = lua_isnil(L, -1);
            if (!missing) {
                cameraComponent.view = lua_tomat4(L, -1);
            }
            lua_pop(L, 1);
        }
        return 0;
    }


    // Lua stack
    // - [-1] number    Entity
    // - [-2] userdata  Binding
    int EntityLuaBinding::getPosition(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -1);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -2);
        auto& transformComponent = binding->scene->entityRegistry.get<TransformComponent>(entity);
        lua_pushvec3(L, transformComponent.position);
        return 1;
    }

    // Lua stack
    // - [-1] table    Position vector
    // - [-2] number   Entity
    // - [-3] userdata Binding
    int EntityLuaBinding::setPosition(lua_State* L) {
        entt::entity entity = (entt::entity) lua_tonumber(L, -2);
        auto* binding = (EntityLuaBinding*) lua_touserdata(L, -3);
        auto& transformComponent = binding->scene->entityRegistry.get<TransformComponent>(entity);
        transformComponent.position = lua_tovec3(L, -1);
        return 0;
    }

    // Lua stack
    // - [-1] string   Entity tag
    // - [-2] userdata Binding
    int EntityLuaBinding::getEntityByTag(lua_State* L) {
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
