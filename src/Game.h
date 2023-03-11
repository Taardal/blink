#pragma once

#include "window/Window.h"
#include "graphics/Renderer.h"
#include "LuaEngine.h"
#include "AppConfig.h"

namespace Blink {
    class Game {
    private:
        Window *window;
        Renderer *renderer;

    public:
        Game(Window *window, Renderer *renderer);

        void run();

    private:
        void onUpdate() const;

        void onRender() const;
    };
}