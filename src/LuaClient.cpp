#include "LuaClient.h"
#include "Log.h"
#include <cassert>
#include <sstream>

LuaClient::LuaClient()
        : L(luaL_newstate()), canClearStack(true)
{}

LuaClient::~LuaClient()
{
    lua_close(L);
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
        ST_LOG_ERROR(ST_TAG, "Could not run file [{0}]", GetString());
        assert(result != LUA_OK);
    }
}

void LuaClient::RunScript(const std::string& script) const
{
    ST_LOG_DEBUG(ST_TAG, "Running script [{0}]", script);
    int result = luaL_dostring(L, script.c_str());
    if (result != LUA_OK)
    {
        ST_LOG_ERROR(ST_TAG, "Could not run script [{0}]", GetString());
        assert(result != LUA_OK);
    }
}

void LuaClient::PutGlobalOnStack(const std::string& name) const
{
    lua_getglobal(L, name.c_str());
}

void LuaClient::PushFunction(const std::string& name, int (*function)(lua_State*)) const
{
    lua_pushcfunction(L, function);
}

const char* LuaClient::GetString() const
{
    return lua_tostring(L, -1);
}

void LuaClient::PrintStack(const std::string& tag) const
{
    if (!tag.empty())
    {
        printf("%s\n", tag.c_str());
    }
    printf("-- lua stack\n");
    int stackSize = lua_gettop(L);
    if (stackSize == 0)
    {
        printf("--   empty\n");
    }
    else
    {
        for (int i = 1; i <= stackSize; i++)
        {
            int stackIndex = i * -1;
            printf("--   [%d] ", stackIndex);
            if (lua_isuserdata(L, stackIndex))
            {
                printf("userdata\n");
            }
            else if (lua_islightuserdata(L, stackIndex))
            {
                printf("light userdata\n");
            }
            else if (lua_istable(L, stackIndex))
            {
                printf("table\n");
            }
            else if (lua_isfunction(L, stackIndex))
            {
                printf("function\n");
            }
            else if (lua_iscfunction(L, stackIndex))
            {
                printf("cfunction\n");
            }
            else if (lua_isthread(L, stackIndex))
            {
                printf("thread\n");
            }
            else if (lua_isstring(L, stackIndex))
            {
                printf("string\n");
            }
            else if (lua_isnumber(L, stackIndex))
            {
                printf("number\n");
            }
            else if (lua_isnone(L, stackIndex))
            {
                printf("none\n");
            }
            else if (lua_isnil(L, stackIndex))
            {
                printf("nil\n");
            }
            else
            {
                printf("unknown type\n");
            }
        }
    }
    printf("-- end\n");
}
