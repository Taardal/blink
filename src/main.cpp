#include "pch.h"
#include "App.h"

using namespace Blink;

int main() {
    initializeErrorSignalHandlers();
    Log::initialize(LogLevel::Debug);

    AppConfig config{};
    config.name = "Blink";
    config.windowWidth = 1366;
    config.windowHeight = 768;
    config.windowResizable = true;
    config.windowMaximized = true;
    config.scenes = {
        "lua/scenes/sandbox/sandbox.out",
        "lua/scenes/viking_room/viking_room.out",
        "lua/scenes/rotation_test/rotation_test.out",
    };

    App app(config);
    app.run();
}
