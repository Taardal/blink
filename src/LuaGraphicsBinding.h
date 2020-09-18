#pragma once

#include "LuaBindingListener.h"
#include "LuaClient.h"

namespace blink
{
    class LuaGraphicsBinding
    {
    private:
        LuaClient* lua;
        LuaBindingListener* listener;

    public:
        explicit LuaGraphicsBinding(LuaClient* lua);

        void Init(LuaBindingListener* listener);

    private:
        static int DrawRectangle(lua_State* L);
    };
}


