#include "pch.h"
#include "lua/LuaEngine.h"
#include "lua/luaUtils.h"
#include "lua/CoordinateSystemLuaBinding.h"
#include "lua/EntityLuaBinding.h"
#include "lua/GlmLuaBinding.h"
#include "lua/KeyboardLuaBinding.h"
#include "lua/SceneCameraLuaBinding.h"
#include "lua/SkyboxLuaBinding.h"
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

    void LuaEngine::clear() {
        terminate();
        initialize();
    }

    void LuaEngine::initializeCoreBindings(Scene* scene) const {
        CoordinateSystemLuaBinding::initialize(L);
        EntityLuaBinding::initialize(L, scene);
        GlmLuaBinding::initialize(L);
        KeyboardLuaBinding::initialize(L, config.keyboard);
        SceneCameraLuaBinding::initialize(L, config.sceneCamera);
        SkyboxLuaBinding::initialize(L, scene);
        WindowLuaBinding::initialize(L, config.window);
    }

    void LuaEngine::initializeEntityBinding(entt::entity entity, const LuaComponent& luaComponent, const TagComponent& tagComponent) const {
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
            BL_THROW("Could not initialize entity binding");
        }
        BL_LOG_INFO(
            "Loaded Lua script [{}] for entity [id: {}, type: {}, tag: {}]",
            filepath,
            entity,
            tableName,
            tagComponent.tag
        );
    }

    void LuaEngine::configureSkybox(const std::string& sceneFilePath) const {
        static const char* tableName = "Scene";
        static const char* functionName = "onConfigureSkybox";

        lua_newtable(L);
        lua_setglobal(L, tableName);

        if (luaL_dofile(L, sceneFilePath.c_str()) != LUA_OK) {
            const char* errorMessage = lua_tostring(L, -1);
            BL_LOG_ERROR("Could not load Lua script [{}]: {}", sceneFilePath, errorMessage);
            BL_THROW("Could not load Lua script");
        }
        BL_LOG_INFO("Loaded Lua script [{}]", sceneFilePath);

        lua_pushcfunction(L, printLuaError);
        lua_getglobal(L, tableName);
        lua_getfield(L, -1, functionName);

        bool functionMissing = lua_isnil(L, -1);
        if (!functionMissing) {
            constexpr int argumentCount = 0;
            constexpr int returnValueCount = 1;
            constexpr int errorHandlerIndex = -3;
            if (lua_pcall(L, argumentCount, returnValueCount, errorHandlerIndex) != LUA_OK) {
                const char* errorMessage = lua_tostring(L, -1);
                BL_LOG_ERROR(
                    "Could not invoke [{}:{}:{}]: {}",
                    sceneFilePath,
                    tableName,
                    functionName,
                    errorMessage
                );
                BL_THROW("Could not configure scene camera");
            }
        }

        lua_pop(L, lua_gettop(L));
    }

    void LuaEngine::configureSceneCamera(const std::string& sceneFilePath) const {
        static const char* tableName = "Scene";
        static const char* functionName = "onConfigureCamera";

        lua_newtable(L);
        lua_setglobal(L, tableName);

        if (luaL_dofile(L, sceneFilePath.c_str()) != LUA_OK) {
            const char* errorMessage = lua_tostring(L, -1);
            BL_LOG_ERROR("Could not load Lua script [{}]: {}", sceneFilePath, errorMessage);
            BL_THROW("Could not load Lua script");
        }
        BL_LOG_INFO("Loaded Lua script [{}]", sceneFilePath);

        lua_pushcfunction(L, printLuaError);
        lua_getglobal(L, tableName);
        lua_getfield(L, -1, functionName);

        bool functionMissing = lua_isnil(L, -1);
        if (!functionMissing) {
            constexpr int argumentCount = 0;
            constexpr int returnValueCount = 0;
            constexpr int errorHandlerIndex = -3;
            if (lua_pcall(L, argumentCount, returnValueCount, errorHandlerIndex) != LUA_OK) {
                const char* errorMessage = lua_tostring(L, -1);
                BL_LOG_ERROR(
                    "Could not invoke [{}:{}:{}]: {}",
                    sceneFilePath,
                    tableName,
                    functionName,
                    errorMessage
                );
                BL_THROW("Could not configure scene camera");
            }
        }

        lua_pop(L, lua_gettop(L));
    }

    void LuaEngine::createEntities(const std::string& sceneFilePath) const {
        static const char* tableName = "Scene";
        static const char* functionName = "onCreateEntities";

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

        lua_pushcfunction(L, printLuaError);
        lua_getglobal(L, tableName);
        lua_getfield(L, -1, functionName);

        constexpr int argumentCount = 0;
        constexpr int returnValueCount = 0;
        constexpr int errorHandlerIndex = -3;

        if (lua_pcall(L, argumentCount, returnValueCount, errorHandlerIndex) != LUA_OK) {
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

    void LuaEngine::updateEntity(entt::entity entity, const LuaComponent& luaComponent, const TagComponent& tagComponent, double timestep) const {
        static const char* functionName = "onUpdate";
        std::string tableName = luaComponent.type;

        lua_pushcfunction(L, printLuaError);
        lua_getglobal(L, tableName.c_str());
        lua_getfield(L, -1, functionName);
        lua_pushnumber(L, (uint32_t) entity);
        lua_pushnumber(L, timestep);

        constexpr int argumentCount = 2;
        constexpr int returnValueCount = 0;
        constexpr int errorHandlerIndex = -5;

        if (lua_pcall(L, argumentCount, returnValueCount, errorHandlerIndex) != LUA_OK) {
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
            BL_THROW("Could not update entity");
        }

        lua_pop(L, lua_gettop(L));
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

    void LuaEngine::initialize() {
        L = luaL_newstate();

        // Enable Lua standard libraries
        luaL_openlibs(L);

        // Add paths to be searched for lua files
        luaL_dostring(L, "package.path = './lua/?.out;' .. package.path");

        // Override Lua 'print' function with custom logger
        lua_pushcfunction(L, LuaEngine::printLuaMessage);
        lua_setglobal(L, "print");
    }

    void LuaEngine::terminate() const {
        lua_close(L);
    }

    int LuaEngine::printLuaMessage(lua_State* L) {
        const char* msg = lua_tostring(L, -1);
        BL_LOG_INFO("[LUA] - {}", msg);
        return 0;
    }

    int LuaEngine::printLuaError(lua_State* L) {
        return printLuaStacktrace(L);
    }
}
