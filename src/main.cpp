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
    config.fps = 120;

    config.scene = "lua/scenes/rotation_test/rotation_test.out";
    config.scene = "lua/scenes/viking_room/viking_room.out";
    config.scene = "lua/scenes/sandbox/sandbox.out";

    App app(config);
    app.run();
}
