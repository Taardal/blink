#include "App.h"

int main() {
    Blink::AppConfig config;
    config.name = "Blink";
    config.logLevel = Blink::LogLevel::Debug;
    config.windowWidth = 1280;
    config.windowHeight = 768;
    config.windowResizable = true;

    Blink::runApp(config);

    return EXIT_SUCCESS;
}