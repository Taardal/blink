#include "LuaClient.h"
#include "Entity.h"
#include "World.h"
#include "FileSystem.h"
#include "lua/lua.hpp"
#include <iostream>
#include <cassert>

using namespace blink;

void Lua_InitEntity(lua_State* L)
{
    lua_newtable(L);
    int entityTableIndex = lua_gettop(L);
    lua_pushvalue(L, entityTableIndex);
    lua_setglobal(L, "Entity");

    lua_pushcfunction(L, [](lua_State* L) {
        void* entity = lua_newuserdata(L, sizeof(Entity));
        new(entity) Entity();
        luaL_getmetatable(L, "EntityMetatable");
        lua_setmetatable(L, -2);
        return 1;
    });
    lua_setfield(L, -2, "New");

    lua_pushcfunction(L, [](lua_State* L) {
        auto entity = (Entity*) lua_touserdata(L, -3);
        int velocityX = (int) lua_tonumber(L, -2);
        int velocityY = (int) lua_tonumber(L, -1);
        entity->Update(velocityX, velocityY);
        return 0;
    });
    lua_setfield(L, -2, "Update");

    lua_pushcfunction(L, [](lua_State* L) {
        auto entity = (Entity*) lua_touserdata(L, -1);
        entity->Draw();
        return 0;
    });
    lua_setfield(L, -2, "Draw");

    luaL_newmetatable(L, "EntityMetatable");

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, [](lua_State* L) {
        auto entity = (Entity*) lua_touserdata(L, -1);
        entity->~Entity();
        return 0;
    });
    lua_settable(L, -3);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, entityTableIndex);
    lua_settable(L, -3);
}

void Lua_InitWorld(lua_State* L)
{
    lua_newtable(L);
    int tableIndex = lua_gettop(L);
    lua_pushvalue(L, tableIndex);
    lua_setglobal(L, "World");

    lua_pushcfunction(L, [](lua_State* L) {
        const char* path = lua_tostring(L, -1);
        void* world = lua_newuserdata(L, sizeof(World));
        new(world) World(path);
        luaL_getmetatable(L, "WorldMetatable");
        lua_setmetatable(L, -2);
        return 1;
    });
    lua_setfield(L, -2, "Load");

    lua_pushcfunction(L, [](lua_State* L) {
        auto world = (World*) lua_touserdata(L, -1);
        world->Update();
        return 0;
    });
    lua_setfield(L, -2, "Update");

    lua_pushcfunction(L, [](lua_State* L) {
        auto world = (World*) lua_touserdata(L, -1);
        world->Draw();
        return 0;
    });
    lua_setfield(L, -2, "Draw");

    luaL_newmetatable(L, "WorldMetatable");

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, [](lua_State* L) {
        auto world = (World*) lua_touserdata(L, -1);
        world->~World();
        return 0;
    });
    lua_settable(L, -3);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, tableIndex);
    lua_settable(L, -3);
}

int main()
{
    lua_State* L = luaL_newstate();
    LuaClient lua(L);

    luaL_openlibs(L);
    luaL_dostring(L, "package.path = package.path .. ';res/?.lua'");

    Lua_InitEntity(L);
    Lua_InitWorld(L);

    {
        int result = luaL_dofile(L, "res/main.lua");
        if (result != LUA_OK)
        {
            std::cout << lua_tostring(L, -1) << std::endl;
        }
    }

    std::cout << "A :: " << lua.GetString("a") << std::endl;

    lua.CallFunction("OnCreate");
    lua.CallFunction("OnUpdate");
    lua.CallFunction("OnDraw");

    return 0;
}