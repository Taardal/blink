#include "pch.h"
#include "LuaEngine.h"
#include "printLua.h"
#include "EntityLuaBinding.h"
#include "KeyboardLuaBinding.h"

#include <lua.hpp>
#include <iostream>

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
        EntityLuaBinding::create(L, entityRegistry);
    }

    void LuaEngine::update(const std::string& tableName, double timestep, entt::entity entity) {
        const char* functionName = "onUpdate";

        lua_getglobal(L, tableName.c_str());

        // - [-1] table     {tableName}

        lua_getfield(L, -1, functionName);

        // - [-1] function  {onUpdate}
        // - [-2] table     {tableName}

        lua_pushnumber(L, timestep);

        // - [-1] number    {timestep}
        // - [-2] function  {onUpdate}
        // - [-3] table     {tableName}

        lua_pushnumber(L, (uint32_t) entity);

        // - [-1] number    {entity}
        // - [-2] number    {timestep}
        // - [-3] function  {onUpdate}
        // - [-4] table     {tableName}

        constexpr int argumentCount = 2;
        constexpr int returnValueCount = 0;
        constexpr int messageHandlerIndex = 0;
        if (lua_pcall(L, argumentCount, returnValueCount, messageHandlerIndex) != LUA_OK) {
            BL_LOG_ERROR("Could not invoke function [{}]: {}", functionName, lua_tostring(L, -1));
            throw std::runtime_error("no worky");
        }

        // - [-1] table     {tableName}

        clearStack();
    }

    void LuaEngine::createKeyboardBinding(Keyboard* keyboard) {
        KeyboardLuaBinding::create(L, keyboard);
    }

    void LuaEngine::clearStack() {
        int i = lua_gettop(L);
        if (i > 0) {
            lua_pop(L, i);
        }
    }

    int LuaEngine::luaPrint(lua_State* L) {
        const char* msg = lua_tostring(L, -1);
        BL_LOG_INFO("[Lua] {}", msg);
        return 0;
    }
}
