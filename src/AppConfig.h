#pragma once

#include "system/Log.h"
#include <string>

namespace Blink {

    struct AppConfig {
        std::string name = "App";
        int32_t windowWidth = 800;
        int32_t windowHeight = 600;
        bool windowMaximized = false;
        bool windowResizable = false;
    };
}


