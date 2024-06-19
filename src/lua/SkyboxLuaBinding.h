#pragma once

#include "scene/Scene.h"

namespace Blink {
    class SkyboxLuaBinding {
    private:
        Scene* scene;

    public:
        explicit SkyboxLuaBinding(Scene* scene);

        static void initialize(lua_State* L, Scene* scene);

    private:
        static int destroy(lua_State* L);

        static int index(lua_State* L);

        static int setSkybox(lua_State* L);
    };
}