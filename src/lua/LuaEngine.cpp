#include "pch.h"
#include "LuaEngine.h"
#include "printLua.h"

#include <lua.hpp>
#include <iostream>

namespace Blink {
    LuaEngine::LuaEngine() : L(luaL_newstate()) {
    }

    LuaEngine::~LuaEngine() {
        lua_close(L);
    }

    bool LuaEngine::initialize() {
        luaL_openlibs(L);

        // Add compiled lua files as paths to be searched when requiring files
        luaL_dostring(L, "package.path = './?.out;' .. package.path");
        luaL_dostring(L, "package.path = './lua/?.out;' .. package.path");

        // Load lua scripts
        // auto luafile = "lua/main.out";
        // if (luaL_loadfile(L, luafile) != LUA_OK) {
        //     BL_LOG_ERROR("Could not load file [{}]: {}", luafile, lua_tostring(L, -1));
        //     return false;
        // }
        // BL_LOG_INFO("Loaded file [{}] successfully", luafile);

        // Run file lua scripts
        // if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        //     BL_LOG_ERROR("Could not execute file [{}]: {}", luafile, lua_tostring(L, -1));
        //     return false;
        // }
        // BL_LOG_INFO("Executed file [{}] successfully", luafile);

        // Call main function
        // auto mainFn = "main";
        // lua_getglobal(L, mainFn);
        // if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        //     BL_LOG_ERROR("Could not invoke function [{}]: {}", "main", lua_tostring(L, -1));
        //     return false;
        // }

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

    void LuaEngine::update(const std::string& tableName) {
        const char* functionName = "onUpdate";
        lua_getglobal(L, tableName.c_str());
        lua_getfield(L, -1, functionName);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            BL_LOG_ERROR("Could not invoke function [{}]: {}", functionName, lua_tostring(L, -1));
            throw std::runtime_error("no worky");
        }
    }
}
