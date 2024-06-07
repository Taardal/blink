#pragma once

#include "lua/EntityLuaBinding.h"
#include "lua/SceneCameraLuaBinding.h"
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

        void reloadLuaScripts(Scene* scene) const;

        void initializeCoreBindings(Scene* scene) const;

        void initializeEntityBindings(Scene* scene) const;

        void configureSceneCamera(const std::string& sceneFilePath) const;

        void createEntities(const std::string& sceneFilePath) const;

        void updateEntities(Scene* scene, double timestep) const;

    private:
        void initialize();

        void terminate() const;

        void compileLuaFiles() const;

        static int printLuaMessage(lua_State* L);

        static int printLuaError(lua_State* L);
    };
}
