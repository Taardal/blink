#include "pch.h"
#include "lua/LuaEngine.h"
#include "lua/luaUtils.h"
#include "lua/EntityLuaBinding.h"
#include "lua/GlmLuaBinding.h"
#include "lua/KeyboardLuaBinding.h"
#include "lua/SceneCameraLuaBinding.h"
#include "lua/WindowLuaBinding.h"
#include "scene/Components.h"
#include "scene/Scene.h"

#include <lua.hpp>

namespace Blink {
    LuaEngine::LuaEngine(const LuaEngineConfig& config) : config(config) {
        initialize();
    }

    LuaEngine::~LuaEngine() {
        terminate();
    }

    void LuaEngine::resetState() {
        terminate();
        initialize();
    }

    void LuaEngine::reloadLuaScripts(Scene* scene) const {
        compileLuaFiles();
        initializeCoreBindings(scene);
        initializeEntityBindings(scene);
    }

    void LuaEngine::initializeCoreBindings(Scene* scene) const {
        EntityLuaBinding::initialize(L, scene);
        GlmLuaBinding::initialize(L);
        KeyboardLuaBinding::initialize(L, config.keyboard);
        SceneCameraLuaBinding::initialize(L, config.sceneCamera);
        WindowLuaBinding::initialize(L, config.window);
    }

    void LuaEngine::initializeEntityBindings(Scene* scene) const {
        for (entt::entity entity : scene->entityRegistry.view<LuaComponent>()) {
            const auto& luaComponent = scene->entityRegistry.get<LuaComponent>(entity);
            const std::string& tableName = luaComponent.type;
            const std::string& filepath = luaComponent.path;

            const auto* tagComponent = scene->entityRegistry.try_get<TagComponent>(entity);
            const std::string& entityTag = tagComponent != nullptr ? tagComponent->tag : "Unknown";

            lua_newtable(L);
            lua_setglobal(L, tableName.c_str());
            if (luaL_dofile(L, filepath.c_str()) != LUA_OK) {
                const char* errorMessage = lua_tostring(L, -1);
                BL_LOG_ERROR(
                    "Could not load Lua script [{}] for entity [id: {}, type: {}, tag: {}]: {}",
                    filepath,
                    tableName,
                    entityTag,
                    errorMessage
                );
                BL_THROW("Could not load Lua script");
            }
            BL_LOG_INFO(
                "Loaded Lua script [{}] for entity [id: {}, type: {}, tag: {}]",
                filepath,
                entity,
                tableName,
                entityTag
            );
        }
    }

    void LuaEngine::configureSceneCamera(const std::string& sceneFilePath) const {
        const char* tableName = "Scene";
        const char* functionName = "onConfigureCamera";

        lua_newtable(L);
        lua_setglobal(L, tableName);
        if (luaL_dofile(L, sceneFilePath.c_str()) != LUA_OK) {
            const char* errorMessage = lua_tostring(L, -1);
            BL_LOG_ERROR(
                "Could not load Lua script [{}]: {}",
                sceneFilePath,
                errorMessage
            );
            BL_THROW("Could not load Lua script");
        }
        BL_LOG_INFO(
            "Loaded Lua script [{}]",
            sceneFilePath
        );

        lua_getglobal(L, tableName);
        lua_getfield(L, -1, functionName);

        bool functionMissing = lua_isnil(L, -1);
        if (!functionMissing) {
            constexpr int argumentCount = 0;
            constexpr int returnValueCount = 0;
            constexpr int messageHandlerIndex = 0;
            if (lua_pcall(L, argumentCount, returnValueCount, messageHandlerIndex) != LUA_OK) {
                const char* errorMessage = lua_tostring(L, -1);
                BL_LOG_ERROR(
                    "Could not invoke [{}:{}:{}]: {}",
                    sceneFilePath,
                    tableName,
                    functionName,
                    errorMessage
                );
                BL_THROW("Could not call Lua function");
            }
        }

        lua_pop(L, lua_gettop(L));
    }

    void LuaEngine::createEntities(const std::string& sceneFilePath) const {
        const char* tableName = "Scene";
        const char* functionName = "onCreateEntities";

        lua_newtable(L);
        lua_setglobal(L, tableName);
        if (luaL_dofile(L, sceneFilePath.c_str()) != LUA_OK) {
            const char* errorMessage = lua_tostring(L, -1);
            BL_LOG_ERROR(
                "Could not load Lua script [{}]: {}",
                sceneFilePath,
                errorMessage
            );
            BL_THROW("Could not load Lua script");
        }
        BL_LOG_INFO(
            "Loaded Lua script [{}]",
            sceneFilePath
        );

        lua_getglobal(L, tableName);
        lua_getfield(L, -1, functionName);
        constexpr int argumentCount = 0;
        constexpr int returnValueCount = 0;
        constexpr int messageHandlerIndex = 0;
        if (lua_pcall(L, argumentCount, returnValueCount, messageHandlerIndex) != LUA_OK) {
            const char* errorMessage = lua_tostring(L, -1);
            BL_LOG_ERROR(
                "Could not invoke [{}:{}:{}]: {}",
                sceneFilePath,
                tableName,
                functionName,
                errorMessage
            );
            BL_THROW("Could not call Lua function");
        }
        lua_pop(L, lua_gettop(L));
    }

    void LuaEngine::updateEntities(Scene* scene, double timestep) const {
        static const char* functionName = "onUpdate";
        for (entt::entity entity : scene->entityRegistry.view<LuaComponent>()) {
            const LuaComponent& luaComponent = scene->entityRegistry.get<LuaComponent>(entity);

            std::string tableName = luaComponent.type;
            lua_getglobal(L, tableName.c_str());
            lua_getfield(L, -1, functionName);
            lua_pushnumber(L, (uint32_t) entity);
            lua_pushnumber(L, timestep);

            constexpr int argumentCount = 2;
            constexpr int returnValueCount = 0;
            constexpr int messageHandlerIndex = 0;
            if (lua_pcall(L, argumentCount, returnValueCount, messageHandlerIndex) != LUA_OK) {
                const auto* tagComponent = scene->entityRegistry.try_get<TagComponent>(entity);
                const std::string& entityTag = tagComponent != nullptr ? tagComponent->tag : "Unknown";
                const char* errorMessage = lua_tostring(L, -1);
                BL_LOG_ERROR(
                    "Could not invoke [{}:{}:{}] for entity [id: {}, tag: {}]: {}",
                    luaComponent.path,
                    luaComponent.type,
                    functionName,
                    entity,
                    entityTag,
                    errorMessage
                );
                BL_THROW("Could not call Lua function");
            }

            lua_pop(L, lua_gettop(L));
        }
    }

    void LuaEngine::initialize() {
        L = luaL_newstate();

        // Enable Lua standard libraries
        luaL_openlibs(L);

        // Add paths to be searched for lua files
        luaL_dostring(L, "package.path = './lua/?.out;' .. package.path");

        // Override Lua 'print' function with custom logger
        lua_pushcfunction(L, LuaEngine::luaPrint);
        lua_setglobal(L, "print");
    }

    void LuaEngine::terminate() const {
        lua_close(L);
    }

    void LuaEngine::compileLuaFiles() const {
        std::stringstream ss;
        ss << "cmake";
        ss << " -D LUA_SOURCE_DIR=" << CMAKE_LUA_SOURCE_DIR;
        ss << " -D LUA_OUTPUT_DIR=" << CMAKE_LUA_OUTPUT_DIR;
        ss << " -P " << CMAKE_SCRIPTS_DIR << "/compile_lua.cmake";
        std::string command = ss.str();
        std::system(command.c_str());
    }

    int LuaEngine::luaPrint(lua_State* L) {
        const char* msg = lua_tostring(L, -1);
        BL_LOG_INFO("[LUA] - {}", msg);
        return 0;
    }
}
