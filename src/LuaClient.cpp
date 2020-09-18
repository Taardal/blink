#include "LuaClient.h"
#include "Log.h"
#include <cassert>
#include <sstream>

LuaClient::LuaClient()
        : L(luaL_newstate())
{}

LuaClient::~LuaClient()
{
    lua_close(L);
}

lua_State* LuaClient::GetState() const
{
    return L;
}

void LuaClient::EnableStandardLibraries()
{
    luaL_openlibs(L);
}

void LuaClient::AddPackagePath(const char* path)
{
    std::stringstream ss;
    ss << "package.path = package.path .. ';" << path << "'";
    luaL_dostring(L, ss.str().c_str());
}

void LuaClient::RunFile(const std::string& path) const
{
    ST_LOG_INFO(ST_TAG, "Running file [{0}]", path);
    int result = luaL_dofile(L, path.c_str());
    if (result != LUA_OK)
    {
        ST_LOG_ERROR(ST_TAG, "Could not run file [{0}]", ToString());
        assert(result != LUA_OK);
    }
}

void LuaClient::RunScript(const std::string& script) const
{
    ST_LOG_DEBUG(ST_TAG, "Running script [{0}]", script);
    int result = luaL_dostring(L, script.c_str());
    if (result != LUA_OK)
    {
        ST_LOG_ERROR(ST_TAG, "Could not run script [{0}]", ToString());
        assert(result != LUA_OK);
    }
}

void LuaClient::PrintStack(const std::string& tag) const
{
    printlua(L, tag);
}

const LuaClient& LuaClient::Pop(int count) const
{
    lua_pop(L, count);
    return *this;
}

const LuaClient& LuaClient::Clear() const
{
    int i = lua_gettop(L);
    if (i > 0)
    {
        lua_pop(L, i);
    }
    return *this;
}

const LuaClient& LuaClient::Global(const char* name) const
{
    lua_getglobal(L, name);
    return *this;
}

const LuaClient& LuaClient::SetGlobal(const char* name) const
{
    lua_setglobal(L, name);
    return *this;
}

const LuaClient& LuaClient::SetGlobalAndKeep(const char* name) const
{
    lua_pushvalue(L, -1);
    SetGlobal(name);
    return *this;
}

const LuaClient& LuaClient::String(const char* string) const
{
    lua_pushstring(L, string);
    return *this;
}

const char* LuaClient::ToString() const
{
    int i = -1;
    assert(lua_isstring(L, i));
    const char* string = lua_tostring(L, i);
    return string;
}

std::string LuaClient::PopToString() const
{
    std::string string = ToString();
    lua_pop(L, 1);
    return string;
}

const LuaClient& LuaClient::Table() const
{
    lua_newtable(L);
    return *this;
}

const LuaClient& LuaClient::Field(const char* name) const
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

const LuaClient& LuaClient::SetField(const char* name) const
{
    for (int i = -2; i >= lua_gettop(L) * -1; i--)
    {
        if (lua_istable(L, i))
        {
            lua_setfield(L, i, name);
            break;
        }
    }
    return *this;
}

const LuaClient& LuaClient::SetFieldAndKeep(const char* name) const
{
    lua_pushvalue(L, -1);
    SetField(name);
    return *this;
}

const LuaClient& LuaClient::CallFunction(const LuaFunctionConfig& config) const
{
    constexpr int ERROR_HANDLER_STACK_INDEX = 0;
    int result = lua_pcall(L, config.ParameterCount, config.ReturnCount, ERROR_HANDLER_STACK_INDEX);
    if (result != LUA_OK)
    {
        ST_LOG_ERROR(ST_TAG, "Could not run function [{0}]", ToString());
        assert(result != LUA_OK);
    }
    return *this;
}

const LuaClient& LuaClient::CallFunction(int returnCount) const
{
    int parameterCount = 0;
    for (int i = -1; i >= lua_gettop(L) * -1; i--)
    {
        if (lua_isfunction(L, i))
        {
            break;
        }
        parameterCount++;
    }
    constexpr int ERROR_HANDLER_STACK_INDEX = 0;
    int result = lua_pcall(L, parameterCount, returnCount, ERROR_HANDLER_STACK_INDEX);
    if (result != LUA_OK)
    {
        ST_LOG_ERROR(ST_TAG, "Could not run function [{0}]", ToString());
        assert(result != LUA_OK);
    }
    return *this;
}

const LuaClient& LuaClient::CallFunctionWithReturnCount(int returnCount) const
{
    CallFunction(returnCount);
    return *this;
}

const LuaClient& LuaClient::PushFunction(const std::string& name, int (*function)(lua_State*)) const
{
    lua_pushcfunction(L, function);
    return *this;
}
