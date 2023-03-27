#include "App.h"

int main(int argc, char* argv[]) {

    for (int i = 0; i < argc; i++) {
        printf("argc [%i/%i], argv [%s]\n", (i + 1), argc, argv[i]);
    }

    Blink::AppConfig config;
    config.logLevel = Blink::LogLevel::Info;
    config.windowTitle = "Blink";
    config.windowWidth = 1280;
    config.windowHeight = 768;
    config.windowResizable = true;

    auto* app = new Blink::App(config);
    app->run();
    delete app;

    return 0;
}