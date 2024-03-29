#include "App.h"

using namespace Blink;

int main() {
    AppConfig config;
    config.name = "Blink";
    config.logLevel = LogLevel::Debug;
    config.windowWidth = 1280;
    config.windowHeight = 768;
    config.windowResizable = true;

    runApp(config);

    return EXIT_SUCCESS;
}