#pragma once

#include "system/Log.h"
#include <string>

namespace Blink {
    struct Config {
        LogLevel logLevel = LogLevel::None;
        std::string windowTitle = "App";
        int windowWidth = 800;
        int windowHeight = 600;
        bool windowMaximized = false;
        bool windowResizable = false;
        bool vulkanValidationLayersEnabled = false;
    };
}


