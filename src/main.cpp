#include "App.h"

int main(int argc, char* argv[]) {

    for (int i = 0; i < argc; i++) {
        printf("argc [%i/%i], argv [%s]\n", (i + 1), argc, argv[i]);
    }

    Blink::Config config;
    config.logLevel = Blink::LogLevel::Debug;
    config.windowTitle = "Blink";
    config.windowWidth = 1280;
    config.windowHeight = 768;
    config.windowMaximized = false;
    config.windowResizable = false;
    config.vulkanValidationLayersEnabled = true;

    auto* app = new Blink::App();
    app->run(config);
    delete app;

    return 0;
}