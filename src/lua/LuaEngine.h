#pragma once

#include "lua/EntityLuaBinding.h"
#include "lua/SceneCameraLuaBinding.h"
#include "scene/Components.h"
#include "window/Keyboard.h"

#include <lua.hpp>

namespace Blink {
    // Forward declaration
    class Scene;

    struct LuaEngineConfig {
        Keyboard* keyboard;
        SceneCamera* sceneCamera;
        Window* window;
    };

    class LuaEngine {
    private:
        LuaEngineConfig config;
        lua_State* L;

    public:
        explicit LuaEngine(const LuaEngineConfig& config);

        ~LuaEngine();

        void resetState();

        void initializeCoreBindings(Scene* scene) const;

        void initializeEntityBinding(entt::entity entity, const LuaComponent& luaComponent, const TagComponent& tagComponent) const;

        std::vector<std::string> getSkybox(const std::string& sceneFilePath) const;

        void configureSceneCamera(const std::string& sceneFilePath) const;

        void createEntities(const std::string& sceneFilePath) const;

        void updateEntity(entt::entity entity, const LuaComponent& luaComponent, const TagComponent& tagComponent, double timestep) const;

        void compileLuaFiles() const;

    private:
        void initialize();

        void terminate() const;

        static int printLuaMessage(lua_State* L);

        static int printLuaError(lua_State* L);
    };
}
