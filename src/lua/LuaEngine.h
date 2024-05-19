#pragma once

#include "lua/EntityLuaBinding.h"
#include "lua/SceneCameraLuaBinding.h"
#include "window/Keyboard.h"

#include <lua.hpp>

namespace Blink {
    struct LuaEngineConfig {
        Keyboard* keyboard;
        SceneCamera* sceneCamera;
    };

    // Forward declaration
    class Scene;

    class LuaEngine {
    private:
        LuaEngineConfig config;
        lua_State* L;

    public:
        explicit LuaEngine(const LuaEngineConfig& config);

        ~LuaEngine();

        void initializeScene(const std::string& sceneLuaFilePath, Scene* scene) const;

        void updateEntities(Scene* scene, double timestep) const;

        void reloadScripts(Scene* scene);

    public:
        void initializeEntities(const std::string& sceneLuaFilePath) const;

        void initializeCoreLuaBindings(Scene* scene) const;

        void initializeEntityLuaBindings(Scene* scene) const;

        void compileLuaFiles() const;

        static int luaPrint(lua_State* L);
    };
}
