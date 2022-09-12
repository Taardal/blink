#pragma once

#include "system/Log.h"
#include "lua/lua.hpp"
#include <string>
#include <cassert>

void printlua(lua_State* L, const std::string& tag = "");

struct LuaFunctionConfig
{
    int ParameterCount = 0;
    int ReturnCount = 0;

    LuaFunctionConfig& WithParameterCount(int count)
    {
        ParameterCount = count;
        return *this;
    }

    LuaFunctionConfig& WithReturnCount(int count)
    {
        ReturnCount = count;
        return *this;
    }
};

class LuaClient
{
private:
    lua_State* L;
    bool canClearStack;

public:
    LuaClient();

    ~LuaClient();

    lua_State* GetL() const;

    void EnableStandardLibraries();

    void AddPackagePath(const char* path);

    void RunFile(const std::string& path) const;

    void RunScript(const std::string& script) const;

    void PushFunction(const std::string& name, int (*function)(lua_State*)) const;

    void PutGlobalOnStack(const std::string& name) const;

    void PrintStack(const std::string& tag = "") const;

    [[nodiscard]] const char* GetString() const;

    template<typename T>
    T GetNumber() const
    {
        return (T) lua_tonumber(L, -1);
    }

    template<typename T>
    T* GetObject() const
    {
        return (T*) lua_touserdata(L, -1);
    }

    LuaClient& Global(const char* name)
    {
        lua_getglobal(L, name);
        return *this;
    }

    void WriteToGlobal(const char* name)
    {
        lua_setglobal(L, name);
        ClearStack();
    }

    LuaClient& Field(const char* name)
    {
        for (int i = -1; i >= lua_gettop(L) * -1; i--)
        {
            if (lua_istable(L, i))
            {
                lua_getfield(L, i, name);
                break;
            }
        }
        return *this;
    }

    void WriteToField(const char* name)
    {
        for (int i = -2; i >= lua_gettop(L) * -1; i--)
        {
            if (lua_istable(L, i))
            {
                lua_setfield(L, i, name);
                break;
            }
        }
        ClearStack();
    }

    LuaClient& String(const char* string)
    {
        lua_pushstring(L, string);
        return *this;
    }

    [[nodiscard]] const char* ToString()
    {
        int i = -1;
        assert(lua_isstring(L, i));
        const char* string = lua_tostring(L, i);
        ClearStack();
        return string;
    }

    template<typename T>
    LuaClient& Number(T number)
    {
        lua_pushnumber(L, number);
        return *this;
    }

    template<typename T>
    T ToNumber()
    {
        int i = -1;
        assert(lua_isnumber(L, i));
        lua_Number luaNumber = lua_tonumber(L, i);
        ClearStack();
        return (T) luaNumber;
    }

    void CallFunction(const LuaFunctionConfig& config = {})
    {
        constexpr int ERROR_HANDLER_STACK_INDEX = 0;
        int result = lua_pcall(L, config.ParameterCount, config.ReturnCount, ERROR_HANDLER_STACK_INDEX);
        if (result != LUA_OK)
        {
            BL_LOG_ERROR("Could not run function [{0}]", GetString());
            assert(result != LUA_OK);
        }
        ClearStack();
    }

    void Begin()
    {
        canClearStack = false;
    }

    void End()
    {
        if (!canClearStack)
        {
            canClearStack = true;
        }
        ClearStack();
    }

    LuaClient* Table()
    {
        lua_newtable(L);
        return this;
    }

    void SetGlobal(const char* name)
    {
        lua_setglobal(L, name);
        ClearStack();
    }

    void Pop(int count = 1)
    {
        lua_pop(L, count);
    }

    void ClearStack()
    {
        if (canClearStack)
        {
            int i = lua_gettop(L);
            if (i > 0)
            {
                lua_pop(L, i);
            }
        }
    }

};