#pragma once

#include "system/Log.h"
#include <string>

namespace Blink {

    struct AppConfig {
        std::string name = "App";
        std::string scene = "lua/scenes/sandbox.out";
        int32_t windowWidth = 800;
        int32_t windowHeight = 600;
        bool windowMaximized = false;
        bool windowResizable = false;
        uint32_t fps = 60;
    };
}


