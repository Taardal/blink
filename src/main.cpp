#include "pch.h"
#include "App.h"

using namespace Blink;

int main() {
    addErrorSignalHandlers();

    Log::setLevel(LogLevel::Debug);

    AppConfig config{};
    config.name = "Blink";
    config.windowWidth = 1280;
    config.windowHeight = 768;
    config.windowResizable = true;
    config.windowMaximized = false;

    //config.scene = "viking_room";
    //config.scene = "fighter_jet";

    App app(config);
    app.run();
}
