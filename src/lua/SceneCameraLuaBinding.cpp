#include "pch.h"
#include "SceneCameraLuaBinding.h"
#include "luaUtils.h"
#include "scene/Components.h"

namespace Blink {
    SceneCameraLuaBinding::SceneCameraLuaBinding(SceneCamera* sceneCamera) : sceneCamera(sceneCamera) {
    }

    void SceneCameraLuaBinding::initialize(lua_State* L, SceneCamera* sceneCamera) {
        std::string typeName = "SceneCamera";
        std::string metatableName = typeName + "__meta";

        // Allocate memory for the C++ object and push a userdata onto the Lua stack
        void* userdata = lua_newuserdata(L, sizeof(SceneCameraLuaBinding));

        // Construct the C++ object in the allocated memory block
        new(userdata) SceneCameraLuaBinding(sceneCamera);

        // Create a new metatable and push it onto the Lua stack
        luaL_newmetatable(L, metatableName.c_str());

        // Set the __gc metamethod of the metatable to binding destroy function
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, SceneCameraLuaBinding::destroy);
        lua_settable(L, -3);

        // Set the __index metamethod of the metatable to binding index function
        lua_pushstring(L, "__index");
        constexpr int upvalueCount = 0;
        lua_pushcclosure(L, SceneCameraLuaBinding::index, upvalueCount);
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
    int SceneCameraLuaBinding::destroy(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -1);
        binding->~SceneCameraLuaBinding();
        return 0;
    }

    // Lua stack
    // - [-1] string    Name of the index being accessed
    // - [-2] userdata  EntityBinding
    int SceneCameraLuaBinding::index(lua_State* L) {
        std::string indexName = lua_tostring(L, -1);
        if (indexName == "setPosition") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setPosition);
            return 1;
        }
        if (indexName == "setForwardDirection") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setForwardDirection);
            return 1;
        }
        if (indexName == "setRightDirection") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setRightDirection);
            return 1;
        }
        if (indexName == "setUpDirection") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setUpDirection);
            return 1;
        }
        if (indexName == "setWorldUpDirection") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setWorldUpDirection);
            return 1;
        }
        if (indexName == "setFrustum") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setFrustum);
            return 1;
        }
        if (indexName == "setMoveSpeed") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setMoveSpeed);
            return 1;
        }
        if (indexName == "setRotationSpeed") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setRotationSpeed);
            return 1;
        }
        BL_LOG_WARN("Could not resolve index [{}]", indexName);
        return 0;
    }

    // Lua stack
    // - [-1] table    Position vector
    // - [-2] userdata Scene camera binding
    int SceneCameraLuaBinding::setPosition(lua_State* L) {
        lua_getfield(L, -1, "x");
        auto x = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "y");
        auto y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "z");
        auto z = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->setPosition({ x, y, z });
        return 0;
    }

    // Lua stack
    // - [-1] table    Direction vector
    // - [-2] userdata Scene camera binding
    int SceneCameraLuaBinding::setWorldUpDirection(lua_State* L) {
        lua_getfield(L, -1, "x");
        auto x = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "y");
        auto y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "z");
        auto z = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->setWorldUpDirection({ x, y, z });
        return 0;
    }

    // Lua stack
    // - [-1] table    Direction vector
    // - [-2] userdata Scene camera binding
    int SceneCameraLuaBinding::setForwardDirection(lua_State* L) {
        lua_getfield(L, -1, "x");
        auto x = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "y");
        auto y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "z");
        auto z = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->setForwardDirection({ x, y, z });
        return 0;
    }

    // Lua stack
    // - [-1] table    Direction vector
    // - [-2] userdata Scene camera binding
    int SceneCameraLuaBinding::setRightDirection(lua_State* L) {
        lua_getfield(L, -1, "x");
        auto x = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "y");
        auto y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "z");
        auto z = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->setRightDirection({ x, y, z });
        return 0;
    }

    // Lua stack
    // - [-1] table    Direction vector
    // - [-2] userdata Scene camera binding
    int SceneCameraLuaBinding::setUpDirection(lua_State* L) {
        lua_getfield(L, -1, "x");
        auto x = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "y");
        auto y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "z");
        auto z = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->setUpDirection({ x, y, z });
        return 0;
    }

    // Lua stack
    // - [-1] table    Frustum object
    // - [-2] userdata Scene camera binding
    int SceneCameraLuaBinding::setFrustum(lua_State* L) {
        lua_getfield(L, -1, "fieldOfView");
        auto fieldOfView = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "nearClip");
        auto nearClip = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "farClip");
        auto farClip = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        Frustum frustum{};
        frustum.fieldOfView = fieldOfView;
        frustum.nearClip = nearClip;
        frustum.farClip = farClip;

        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->setFrustum(frustum);
        return 0;
    }

    // Lua stack
    // - [-1] number   Move speed
    // - [-2] userdata Scene camera binding
    int SceneCameraLuaBinding::setMoveSpeed(lua_State* L) {
        auto moveSpeed = (float) lua_tonumber(L, -1);
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->setMoveSpeed(moveSpeed);
        return 0;
    }

    // Lua stack
    // - [-1] number   Rotation speed
    // - [-2] userdata Scene camera binding
    int SceneCameraLuaBinding::setRotationSpeed(lua_State* L) {
        auto rotationSpeed = (float) lua_tonumber(L, -1);
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->setRotationSpeed(rotationSpeed);
        return 0;
    }
}
