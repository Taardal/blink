#include "pch.h"
#include "lua/LuaEngine.h"
#include "lua/luaUtils.h"
#include "lua/EntityLuaBinding.h"
#include "lua/GlmLuaBinding.h"
#include "lua/KeyboardLuaBinding.h"
#include "scene/Components.h"
#include "scene/Scene.h"

#include <lua.hpp>

namespace Blink {
    LuaEngine::LuaEngine(const LuaEngineConfig& config) : config(config), L(luaL_newstate()) {
        // Enable Lua standard libraries
        luaL_openlibs(L);

        // Add paths to be searched for lua files
        luaL_dostring(L, "package.path = './lua/?.out;' .. package.path");

        // Override Lua 'print' function with custom logger
        lua_pushcfunction(L, LuaEngine::luaPrint);
        lua_setglobal(L, "print");
    }

    LuaEngine::~LuaEngine() {
        lua_close(L);
    }

    void LuaEngine::initializeScene(const std::string& sceneLuaFilePath, Scene* scene) const {
        // Core bindings used by Lua scripts
        initializeCoreLuaBindings(scene);

        // Run Lua-script that creates entities for the scene, and initialize them with components
        // Requires core bindings
        initializeEntities(sceneLuaFilePath);

        // Bindings to entities' associated Lua-script to be invoked each game update
        // Requires entities to have been created and initialized with components
        initializeEntityLuaBindings(scene);
    }

    void LuaEngine::initializeCoreLuaBindings(Scene* scene) const {
        EntityLuaBinding::initialize(L, scene);
        GlmLuaBinding::initialize(L);
        KeyboardLuaBinding::initialize(L, config.keyboard);
        SceneCameraLuaBinding::initialize(L, config.sceneCamera);
    }

    void LuaEngine::initializeEntities(const std::string& sceneLuaFilePath) const {
        const char* tableName = "Scene";
        const char* functionName = "onCreate";

        lua_newtable(L);
        lua_setglobal(L, tableName);
        if (luaL_dofile(L, sceneLuaFilePath.c_str()) != LUA_OK) {
            const char* errorMessage = lua_tostring(L, -1);
            BL_LOG_ERROR(
                "Could not load Lua script [{}]: {}",
                sceneLuaFilePath,
                errorMessage
            );
            BL_THROW("Could not load Lua script");
        }
        BL_LOG_INFO(
            "Loaded Lua script [{}]",
            sceneLuaFilePath
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
                sceneLuaFilePath,
                tableName,
                functionName,
                errorMessage
            );
            BL_THROW("Could not call Lua function");
        }
        lua_pop(L, lua_gettop(L));
    }

    void LuaEngine::initializeEntityLuaBindings(Scene* scene) const {
        for (entt::entity entity : scene->entityRegistry.view<LuaComponent>()) {
            const auto& luaComponent = scene->entityRegistry.get<LuaComponent>(entity);
            const auto& tagComponent = scene->entityRegistry.get<TagComponent>(entity);
            const std::string& tableName = luaComponent.type;
            const std::string& filepath = luaComponent.path;
            lua_newtable(L);
            lua_setglobal(L, tableName.c_str());
            if (luaL_dofile(L, filepath.c_str()) != LUA_OK) {
                const char* errorMessage = lua_tostring(L, -1);
                BL_LOG_ERROR(
                    "Could not load Lua script [{}] for entity [id: {}, type: {}, tag: {}]: {}",
                    filepath,
                    tableName,
                    tagComponent.tag,
                    errorMessage
                );
                BL_THROW("Could not load Lua script");
            }
            BL_LOG_INFO(
                "Loaded Lua script [{}] for entity [id: {}, type: {}, tag: {}]",
                filepath,
                entity,
                tableName,
                tagComponent.tag
            );
        }
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
                auto& tagComponent = scene->entityRegistry.get<TagComponent>(entity);
                const char* errorMessage = lua_tostring(L, -1);
                BL_LOG_ERROR(
                    "Could not invoke [{}:{}:{}] for entity [id: {}, tag: {}]: {}",
                    luaComponent.path,
                    luaComponent.type,
                    functionName,
                    entity,
                    tagComponent.tag,
                    errorMessage
                );
                BL_THROW("Could not call Lua function");
            }

            lua_pop(L, lua_gettop(L));
        }
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
