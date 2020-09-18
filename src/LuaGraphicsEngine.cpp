#include "LuaGraphicsEngine.h"
#include <glm/glm.hpp>

namespace blink
{
    LuaGraphicsEngine::LuaGraphicsEngine(LuaClient* lua)
            : lua(lua),
              listener(nullptr)
    {
    }

    void LuaGraphicsEngine::Init(LuaEngineListener* listener)
    {
        this->listener = listener;

        lua_State* L = lua->GetL();

        lua->ClearStack();
        lua_getglobal(L, "blink");

        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, "graphics");

        int numberOfUpvalues = 1;
        lua_pushlightuserdata(L, listener);
        lua_pushcclosure(L, DrawRectangle, numberOfUpvalues);
        lua_setfield(L, -2, "drawRectangle");
    }

    int LuaGraphicsEngine::DrawRectangle(lua_State* L)
    {
        printlua(L);

        ST_LOG_TRACE(ST_TAG_TYPE(LuaGraphicsEngine), "LuaGraphicsEngine::DrawRectangle");
        auto listener = (LuaEngineListener*) lua_touserdata(L, lua_upvalueindex(1));

        lua_getfield(L, -1, "x");
        auto x = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "y");
        auto y = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "width");
        auto width = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "height");
        auto height = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "r");
        auto r = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "g");
        auto g = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "b");
        auto b = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "a");
        auto a = (float) lua_tonumber(L, -1);
        lua_pop(L, 1);

        glm::vec2 position = { x, y };
        glm::vec2 size = { width, height };
        glm::vec4 color = { r, g, b, a };

        listener->OnDrawRectangle(position, size, color);
        printf("End rendering...\n");
        return 0;
    }
}
