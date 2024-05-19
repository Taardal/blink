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
    };

    class LuaEngine {
    private:
        LuaEngineConfig config;
        lua_State* L;

    public:
        explicit LuaEngine(const LuaEngineConfig& config);

        ~LuaEngine();

        void initializeCoreBindings(Scene* scene) const;

        void initializeEntityBindings(Scene* scene) const;

        void createEntities(const std::string& sceneLuaFilePath) const;

        void updateEntities(Scene* scene, double timestep) const;

        void compileLuaFiles() const;

    private:
        static int luaPrint(lua_State* L);
    };
}
