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
        if (indexName == "setYaw") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setYaw);
            return 1;
        }
        if (indexName == "setPitch") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setPitch);
            return 1;
        }
        if (indexName == "setRoll") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setRoll);
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
        if (indexName == "getFieldOfView") {
            lua_pushcfunction(L, SceneCameraLuaBinding::getFieldOfView);
            return 1;
        }
        if (indexName == "setFieldOfView") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setFieldOfView);
            return 1;
        }
        if (indexName == "getNearClip") {
            lua_pushcfunction(L, SceneCameraLuaBinding::getNearClip);
            return 1;
        }
        if (indexName == "setNearClip") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setNearClip);
            return 1;
        }
        if (indexName == "getFarClip") {
            lua_pushcfunction(L, SceneCameraLuaBinding::getFarClip);
            return 1;
        }
        if (indexName == "setFarClip") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setFarClip);
            return 1;
        }
        if (indexName == "setFrustum") {
            lua_pushcfunction(L, SceneCameraLuaBinding::setFrustum);
            return 1;
        }
        BL_LOG_WARN("Could not resolve index [{}]", indexName);
        return 0;
    }

    // Lua stack
    //
    // SceneCamera:setPosition({ x, y, z }) / SceneCamera:setPosition(glm.vec3(x, y, z))
    // - [-1] table    Position vector
    // - [-2] userdata Binding
    //
    // SceneCamera:setPosition(x, y, z)
    // - [-1] number    Z
    // - [-2] number    Y
    // - [-3] number    X
    // - [-4] userdata Binding
    int SceneCameraLuaBinding::setPosition(lua_State* L) {
        uint32_t argumentCount = lua_gettop(L) - 1;

        bool argumentIsVector = argumentCount == 1;
        bool argumentIsScalars = argumentCount == 3;
        BL_ASSERT_THROW(argumentIsVector || argumentIsScalars);

        int bindingIndex = -lua_gettop(L); // Bottom of the stack
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, bindingIndex);

        glm::vec3& position = binding->sceneCamera->position;

        if (argumentIsVector) {
            lua_getfield(L, -1, "x");
            position.x = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "y");
            position.y = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "z");
            position.z = (float) lua_tonumber(L, -1);
            lua_pop(L, 1);
        }
        if (argumentIsScalars) {
            position.x = (float) lua_tonumber(L, -3);
            position.y = (float) lua_tonumber(L, -2);
            position.z = (float) lua_tonumber(L, -1);
        }

        return 0;
    }

    // Lua stack
    // - [-1] table    Direction vector
    // - [-2] userdata Binding
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
        binding->sceneCamera->worldUpDirection = { x, y, z };
        return 0;
    }

    // Lua stack
    // - [-1] table    Direction vector
    // - [-2] userdata Binding
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
        binding->sceneCamera->forwardDirection = { x, y, z };
        return 0;
    }

    // Lua stack
    // - [-1] table    Direction vector
    // - [-2] userdata Binding
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
        binding->sceneCamera->rightDirection = { x, y, z };
        return 0;
    }

    // Lua stack
    // - [-1] table    Direction vector
    // - [-2] userdata Binding
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
        binding->sceneCamera->upDirection = { x, y, z };
        return 0;
    }

    // Lua stack
    // - [-1] number   Yaw
    // - [-2] userdata Binding
    int SceneCameraLuaBinding::setYaw(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->yaw = (float) lua_tonumber(L, -1);
        return 0;
    }

    // Lua stack
    // - [-1] number   Pitch
    // - [-2] userdata Binding
    int SceneCameraLuaBinding::setPitch(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->pitch = (float) lua_tonumber(L, -1);
        return 0;
    }

    // Lua stack
    // - [-1] number   Roll
    // - [-2] userdata Binding
    int SceneCameraLuaBinding::setRoll(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->roll = (float) lua_tonumber(L, -1);
        return 0;
    }

    // Lua stack
    // - [-1] number   Move speed
    // - [-2] userdata Binding
    int SceneCameraLuaBinding::setMoveSpeed(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->moveSpeed = (float) lua_tonumber(L, -1);
        return 0;
    }

    // Lua stack
    // - [-1] number   Rotation speed
    // - [-2] userdata Binding
    int SceneCameraLuaBinding::setRotationSpeed(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->rotationSpeed = (float) lua_tonumber(L, -1);
        return 0;
    }

    // Lua stack
    // - [-1] userdata Binding
    int SceneCameraLuaBinding::getFieldOfView(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -1);
        lua_pushnumber(L, binding->sceneCamera->fieldOfView);
        return 1;
    }

    // Lua stack
    // - [-1] number   Field of view (radians)
    // - [-2] userdata Binding
    int SceneCameraLuaBinding::setFieldOfView(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->fieldOfView = (float) lua_tonumber(L, -1);
        return 0;
    }

    // Lua stack
    // - [-1] userdata Binding
    int SceneCameraLuaBinding::getNearClip(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -1);
        lua_pushnumber(L, binding->sceneCamera->nearClip);
        return 1;
    }

    // Lua stack
    // - [-1] number   Near clip
    // - [-2] userdata Binding
    int SceneCameraLuaBinding::setNearClip(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->nearClip = (float) lua_tonumber(L, -1);
        return 0;
    }

    // Lua stack
    // - [-1] userdata Binding
    int SceneCameraLuaBinding::getFarClip(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -1);
        lua_pushnumber(L, binding->sceneCamera->farClip);
        return 1;
    }

    // Lua stack
    // - [-1] number   Far clip
    // - [-2] userdata Binding
    int SceneCameraLuaBinding::setFarClip(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);
        binding->sceneCamera->farClip = (float) lua_tonumber(L, -1);
        return 0;
    }

    // Lua stack
    // - [-1] table    Frustum object
    // - [-2] userdata Binding
    int SceneCameraLuaBinding::setFrustum(lua_State* L) {
        auto* binding = (SceneCameraLuaBinding*) lua_touserdata(L, -2);

        lua_getfield(L, -1, "fieldOfView");
        binding->sceneCamera->fieldOfView = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "nearClip");
        binding->sceneCamera->nearClip = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "farClip");
        binding->sceneCamera->farClip = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        return 0;
    }
}
