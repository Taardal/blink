#include "LuaClient.h"
#include "lua/lua.hpp"
#include <iostream>
#include <cassert>

void RunScript(const LuaClient& luaClient, const std::string& script)
{
    std::string error = luaClient.RunScript(script);
    if (!error.empty())
    {
        std::cout << "ERROR: " << error << std::endl;
        assert(error.empty());
    }
}

struct Entity
{
    const char* Name;
    int X;
    int Y;

    Entity()
            : Name("Entity"), X(0), Y(0)
    {
        std::cout << "Created entity" << std::endl;
    }

    ~Entity()
    {
        std::cout << "Destroyed entity" << std::endl;
    }

    void Move(int velocityX, int velocityY)
    {
        X += velocityX;
        Y += velocityY;
        std::cout << "Moved entity to " << X << ", " << Y << std::endl;
    }

    void Draw() const
    {
        std::cout << "Drawed entity at " << X << ", " << Y << std::endl;
    }

    friend std::ostream& operator<<(std::ostream& os, const Entity& entity)
    {
        os << "Name: " << entity.Name << " X: " << entity.X << " Y: " << entity.Y;
        return os;
    }
};

int main()
{
    lua_State* L = luaL_newstate();
    LuaClient lua(L);

    lua_newtable(L);
    int entityTableIndex = lua_gettop(L);
    lua_pushvalue(L, entityTableIndex);
    lua_setglobal(L, "Entity");

    lua_pushcfunction(L, [](lua_State* L) {
        void* entity = lua_newuserdata(L, sizeof(Entity));
        new (entity) Entity();
        luaL_getmetatable(L, "EntityMetatable");
        lua_setmetatable(L, -2);
        return 1;
    });
    lua_setfield(L, -2, "New");

    lua_pushcfunction(L, [](lua_State* L) {
        auto entity = (Entity*) lua_touserdata(L, -3);
        int velocityX = (int) lua_tonumber(L, -2);
        int velocityY = (int) lua_tonumber(L, -1);
        entity->Move(velocityX, velocityY);
        return 0;
    });
    lua_setfield(L, -2, "Move");

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

    RunScript(lua, R"(

        function OnCreate()
            player = Entity.New()
        end

        function OnUpdate()
            player:Move(5, 7)
        end

        function OnDraw()
            player:Draw()
        end

    )");

    lua.CallFunction("OnCreate");
    lua.CallFunction("OnUpdate");
    lua.CallFunction("OnDraw");

    auto* player = lua.GetObject<Entity>("player");
    std::cout << "Player: " << *(player) << std::endl;

    return 0;
}