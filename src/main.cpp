#include "pch.h"
#include "App.h"

using namespace Blink;

int main() {
    addSignalHandlers();

    Log::setLevel(LogLevel::Debug);

    AppConfig config;
    config.name = "Blink";
    config.windowWidth = 1280;
    config.windowHeight = 768;
    config.windowResizable = true;

    auto app = new App(config);
    app->run();
    delete app;

    return EXIT_SUCCESS;
}
