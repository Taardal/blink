#pragma once

#include "LuaEngineListener.h"
#include "LuaClient.h"

namespace Blink
{
    class LuaGraphicsEngine
    {
    private:
        LuaClient* lua;
        LuaEngineListener* listener;

    public:
        explicit LuaGraphicsEngine(LuaClient* lua);

        void Init(LuaEngineListener* listener);

    private:
        static int DrawRectangle(lua_State* L);
    };
}


