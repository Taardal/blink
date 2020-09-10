#include "Application.h"


namespace blink
{
    Application::Application(Window* window, LuaEngine* luaEngine)
            : window(window),
              luaEngine(luaEngine),
              running(false)
    {}

    void Application::Run()
    {
        ST_LOG_INFO(ST_TAG, "Running application...");
        running = true;

        luaEngine->OnCreate();
        glClearColor(0.8f, 0.2f, 0.3f, 0.2f);

        while (running)
        {
            glClear(GL_COLOR_BUFFER_BIT);
            luaEngine->OnUpdate();
            luaEngine->OnDraw();
            window->OnUpdate();
        }
    }
}
