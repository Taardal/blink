#pragma once

#include "scene/Components.h"
#include "scene/SceneCamera.h"
#include "window/Keyboard.h"
#include "window/Window.h"

#include <lua.hpp>
#include <entt/entt.hpp>

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

        void configureSkybox(const std::string& sceneFilePath) const;

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
