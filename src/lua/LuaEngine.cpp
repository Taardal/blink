#include "pch.h"
#include "LuaEngine.h"
#include "luaUtils.h"
#include "EntityLuaBinding.h"
#include "KeyboardLuaBinding.h"
#include "scene/Components.h"

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

        // Initialize global Lua script bindings
        createGlobalBindings();
    }

    LuaEngine::~LuaEngine() {
        lua_close(L);
    }

    void LuaEngine::reloadScripts(entt::registry* entityRegistry) const {
        compileLuaScripts();
        createEntityBindings(entityRegistry);
        BL_LOG_INFO("Reloaded Lua scripts");
    }

    void LuaEngine::createEntityBindings(entt::registry* entityRegistry) const {
        EntityLuaBinding::initialize(L, entityRegistry);
        for (entt::entity entity : entityRegistry->view<LuaComponent>()) {
            const LuaComponent& luaComponent = entityRegistry->get<LuaComponent>(entity);
            const std::string& tableName = luaComponent.type;
            const std::string& filepath = luaComponent.path;
            lua_newtable(L);
            lua_setglobal(L, tableName.c_str());
            if (luaL_dofile(L, filepath.c_str()) != LUA_OK) {
                BL_LOG_ERROR(
                    "Could not load Lua script [{}] for entity of type [{}]: {}",
                    filepath,
                    tableName,
                    lua_tostring(L, -1)
                );
                BL_THROW("Could not load Lua script");
            }
            BL_LOG_INFO(
                "Loaded Lua script [{}] for entity of type [{}]",
                filepath,
                tableName
            );
        }
    }

    void LuaEngine::updateEntityBindings(entt::registry* entityRegistry, double timestep) const {
        static const char* functionName = "onUpdate";
        for (entt::entity entity : entityRegistry->view<LuaComponent>()) {
            const LuaComponent& luaComponent = entityRegistry->get<LuaComponent>(entity);
            std::string tableName = luaComponent.type;
            lua_getglobal(L, tableName.c_str());
            lua_getfield(L, -1, functionName);
            lua_pushnumber(L, (uint32_t) entity);
            lua_pushnumber(L, timestep);

            constexpr int argumentCount = 2;
            constexpr int returnValueCount = 0;
            constexpr int messageHandlerIndex = 0;
            if (lua_pcall(L, argumentCount, returnValueCount, messageHandlerIndex) != LUA_OK) {
                BL_LOG_ERROR(
                    "Could not invoke [{}:{}:{}] for entity [{}]: {}",
                    luaComponent.path,
                    luaComponent.type,
                    functionName,
                    entity,
                    lua_tostring(L, -1)
                );
                BL_THROW("Could not update Lua script for entity");
            }

            lua_pop(L, lua_gettop(L));
        }
    }

    void LuaEngine::createGlobalBindings() const {
        KeyboardLuaBinding::initialize(L, config.keyboard);
    }

    void LuaEngine::compileLuaScripts() {
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
