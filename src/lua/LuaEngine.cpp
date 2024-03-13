#include "pch.h"
#include "LuaEngine.h"
#include "LuaFunction.h"
#include "luaUtils.h"
#include "EntityLuaBinding.h"
#include "KeyboardLuaBinding.h"

#include <lua.hpp>

namespace Blink {
    LuaEngine::LuaEngine() : L(luaL_newstate()) {
    }

    LuaEngine::~LuaEngine() {
        lua_close(L);
    }

    bool LuaEngine::initialize() {
        // Enable Lua standard libraries
        luaL_openlibs(L);

        // Add compiled lua files as paths to be searched when requiring files
        luaL_dostring(L, "package.path = './?.out;' .. package.path");
        luaL_dostring(L, "package.path = './lua/?.out;' .. package.path");

        // Override Lua 'print' function with custom logger
        lua_pushcfunction(L, LuaEngine::luaPrint);
        lua_setglobal(L, "print");

        return true;
    }

    void LuaEngine::terminate() {
    }

    bool LuaEngine::loadFile(const std::string& path) {
        // Load and execute Lua script to add it the Lua state
        if (luaL_dofile(L, path.c_str()) != LUA_OK) {
            BL_LOG_ERROR("Could not load file [{}]: {}", path, lua_tostring(L, -1));
            return false;
        }
        BL_LOG_INFO("Loaded file [{}] successfully", path);
        return true;
    }

    void LuaEngine::createEntityType(const std::string& typeName) {
        lua_newtable(L);
        lua_setglobal(L, typeName.c_str());
    }

    void LuaEngine::createEntityBinding(entt::registry* entityRegistry) {
        EntityLuaBinding::initialize(L, entityRegistry);
    }

    void LuaEngine::createKeyboardBinding(Keyboard* keyboard) const {
        KeyboardLuaBinding::create(L, keyboard);
    }

    void LuaEngine::update(const std::string& tableName, double timestep, entt::entity entity) {
        static const char* functionName = "onUpdate";

        lua_getglobal(L, tableName.c_str());
        lua_getfield(L, -1, functionName);
        lua_pushnumber(L, timestep);
        lua_pushnumber(L, (uint32_t) entity);

        constexpr int argumentCount = 2;
        constexpr int returnValueCount = 0;
        constexpr int messageHandlerIndex = 0;
        if (lua_pcall(L, argumentCount, returnValueCount, messageHandlerIndex) != LUA_OK) {
            BL_LOG_ERROR("Could not invoke Lua function [{}] on Lua table [{}]: {}", functionName, tableName, lua_tostring(L, -1));
            throw std::runtime_error("Could not invoke Lua function");
        }

        clearStack();
    }

    void LuaEngine::clearStack() const {
        lua_pop(L, lua_gettop(L));
    }

    int LuaEngine::luaPrint(lua_State* L) {
        const char* msg = lua_tostring(L, -1);
        BL_LOG_INFO("[Lua] {}", msg);
        return 0;
    }
}
