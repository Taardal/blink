#pragma once
#include "lua/lua.hpp"
#include <string>
#include <any>


class LuaClient
{
private:
    static const int TOP_OF_STACK;

    lua_State* L;

public:
    LuaClient();

    LuaClient(lua_State* l);

    ~LuaClient();

    [[nodiscard]] std::string RunScript(const std::string& script) const;

    [[nodiscard]] std::string GetString(const std::string& name) const;

    [[nodiscard]] std::string GetString() const;

    [[nodiscard]] float GetFloat(const std::string& name) const;

    [[nodiscard]] lua_Number GetNumber() const;

    void PushFunction(const std::string& name, int (*function)(lua_State*)) const;

    void CallFunction(const std::string& name) const;

    void PutGlobalOnStack(const std::string& name) const;

    template<typename T>
    T* GetObject(const std::string& name) const
    {
        lua_getglobal(L, name.c_str());
        return (T*) lua_touserdata(L, TOP_OF_STACK);
    }

    [[nodiscard]] std::string GetTableField(const std::string& tableName, const std::string& fieldName) const;

    void SetTableField(const std::string& tableName, const std::string& fieldName, const std::string& value) const;

    void Foo() const;
};