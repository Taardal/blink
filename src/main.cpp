#include "Engine.h"

int main(int argc, char* argv[])
{
    for (int i = 0; i < argc; i++)
    {
        printf("argc [%i/%i], argv [%s]\n", (i + 1), argc, argv[i]);
    }
    const char* luaFilePath = argc > 0 ? argv[1] : nullptr;
    if (luaFilePath)
    {
        printf("%s\n", luaFilePath);
    }

    blink::Window::Config windowConfig;
    windowConfig.Title = "Blink";
    windowConfig.Width = 123;
    windowConfig.Height = 50;
    windowConfig.Maximized = false;

    blink::GraphicsContext::Config graphicsConfig;
    graphicsConfig.GlslVersion = "#version 410";
    graphicsConfig.VersionMajor = 4;
    graphicsConfig.VersionMinor = 1;

    blink::Config config;
    config.WindowConfig = windowConfig;
    config.GraphicsConfig = graphicsConfig;
    config.LogLevel = blink::LogLevel::Trace;

    auto* engine = new blink::Engine(luaFilePath, config);
#if 1
    printf("config :: window :: %s, %i, %i\n", config.WindowConfig.Title.c_str(), config.WindowConfig.Width, config.WindowConfig.Height);
    printf("config :: graphics :: %s, %i, %i\n", config.GraphicsConfig.GlslVersion.c_str(), config.GraphicsConfig.VersionMajor, config.GraphicsConfig.VersionMinor);
#endif
#if 1
    engine->Run();
#endif
    delete engine;

    return 0;
}