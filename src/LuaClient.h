#pragma once

#include "Log.h"
#include "LuaPrintStack.h"
#include <lua/lua.hpp>
#include <string>
#include <cassert>

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

public:
    LuaClient();

    ~LuaClient();

    [[nodiscard]] lua_State* GetState() const;

    void EnableStandardLibraries();

    void AddPackagePath(const char* path);

    void RunFile(const std::string& path) const;

    void RunScript(const std::string& script) const;

    void PrintStack(const std::string& tag = "") const;

    const LuaClient& Pop(int count = 1) const;

    const LuaClient& Clear() const;

    const LuaClient& Global(const char* name) const;

    const LuaClient& SetGlobal(const char* name) const;

    const LuaClient& SetGlobalAndKeep(const char* name) const;

    const LuaClient& String(const char* string) const;

    [[nodiscard]] const char* ToString() const;
    
    [[nodiscard]] std::string PopToString() const;

    template<typename T>
    const LuaClient& Number(T number) const
    {
        lua_pushnumber(L, number);
        return *this;
    }

    template<typename T>
    T ToNumber() const
    {
        int i = -1;
        assert(lua_isnumber(L, i));
        lua_Number luaNumber = lua_tonumber(L, i);
        return (T) luaNumber;
    }

    template<typename T>
    T PopToNumber() const
    {
        auto number = ToNumber<T>();
        lua_pop(L, 1);
        return number;
    }

    const LuaClient& Table() const;

    const LuaClient& Field(const char* name) const;

    const LuaClient& SetField(const char* name) const;

    const LuaClient& SetFieldAndKeep(const char* name) const;

    const LuaClient& CallFunction(const LuaFunctionConfig& config = {}) const;

    const LuaClient& CallFunction(int returnCount = 0) const;

    const LuaClient& CallFunctionWithReturnCount(int returnCount) const;

    const LuaClient& PushFunction(const std::string& name, int (*function)(lua_State*)) const;

};