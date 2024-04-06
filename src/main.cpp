#include "pch.h"
#include "AppConfig.h"
#include "App.h"

using namespace Blink;

int main() {
    Log::setLevel(LogLevel::Debug);

    AppConfig config;
    config.name = "Blink";
    config.windowWidth = 1280;
    config.windowHeight = 768;
    config.windowResizable = true;

    App* app = new App(config);
    app->run();
    delete app;

    return EXIT_SUCCESS;
}