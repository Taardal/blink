#pragma once

#include "scene/SceneCamera.h"

#include <entt/entt.hpp>
#include <lua.hpp>

namespace Blink {
    class SceneCameraLuaBinding {
    private:
        SceneCamera* sceneCamera;

    private:
        explicit SceneCameraLuaBinding(SceneCamera* sceneCamera);

        ~SceneCameraLuaBinding() = default;

    public:
        static void initialize(lua_State* L, SceneCamera* sceneCamera);

    private:
        static int destroy(lua_State* L);

        static int index(lua_State* L);

        static int setPosition(lua_State* L);

        static int setWorldUpDirection(lua_State* L);

        static int setForwardDirection(lua_State* L);

        static int setRightDirection(lua_State* L);

        static int setUpDirection(lua_State* L);

        static int setYaw(lua_State* L);

        static int setPitch(lua_State* L);

        static int setRoll(lua_State* L);

        static int setMoveSpeed(lua_State* L);

        static int setRotationSpeed(lua_State* L);

        static int getFieldOfView(lua_State* L);

        static int setFieldOfView(lua_State* L);

        static int getNearClip(lua_State* L);

        static int setNearClip(lua_State* L);

        static int getFarClip(lua_State* L);

        static int setFarClip(lua_State* L);

        static int setFrustum(lua_State* L);
    };
}
