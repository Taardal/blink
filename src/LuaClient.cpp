#include "LuaClient.h"

constexpr int LuaClient::TOP_OF_STACK = -1;

LuaClient::LuaClient()
        : L()
{
}

LuaClient::LuaClient(lua_State* L)
        : L(L)
{}

LuaClient::~LuaClient()
{
    lua_close(L);
}

std::string LuaClient::RunScript(const std::string& script) const
{
    int result = luaL_dostring(L, script.c_str());
    if (result != LUA_OK)
    {
        return GetString();
    }
    return "";
}

std::string LuaClient::GetString(const std::string& name) const
{
    PutGlobalOnStack(name);
    return GetString();
}

float LuaClient::GetFloat(const std::string& name) const
{
    PutGlobalOnStack(name);
    return (float) GetNumber();
}

std::string LuaClient::GetString() const
{
    return lua_tostring(L, TOP_OF_STACK);
}

lua_Number LuaClient::GetNumber() const
{
    return lua_tonumber(L, TOP_OF_STACK);
}

void LuaClient::PutGlobalOnStack(const std::string& name) const
{
    lua_getglobal(L, name.c_str());
}

void LuaClient::PushFunction(const std::string& name, int (*function)(lua_State*)) const
{
    auto luaF = [function](lua_State* L) {
        function(L);
    };
    lua_pushcfunction(L, function);
    lua_setglobal(L, name.c_str());
}

void LuaClient::CallFunction(const std::string& name) const
{
    PutGlobalOnStack(name);
    constexpr int PARAMETER_COUNT = 0;
    constexpr int RETURN_VALUE_COUNT = 1;
    constexpr int ERROR_HANDLER = 0;
    lua_pcall(L, PARAMETER_COUNT, RETURN_VALUE_COUNT, ERROR_HANDLER);
}

std::string LuaClient::GetTableField(const std::string& tableName, const std::string& fieldName) const
{
    lua_getglobal(L, tableName.c_str());
    lua_pushstring(L, fieldName.c_str());
    lua_gettable(L, -2);
    return GetString();
}

void LuaClient::SetTableField(const std::string& tableName, const std::string& fieldName, const std::string& value) const
{
    lua_getglobal(L, tableName.c_str());
    lua_pushstring(L, value.c_str());
    lua_setfield(L, -2, fieldName.c_str());
}

void LuaClient::Foo() const
{


}
