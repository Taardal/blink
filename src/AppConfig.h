#pragma once

#include "system/Log.h"
#include <string>
#include <vector>

namespace Blink {

    struct AppConfig {
        LogLevel logLevel = LogLevel::Trace;
        std::string windowTitle = "App";
        int32_t windowWidth = 800;
        int32_t windowHeight = 600;
        bool windowMaximized = false;
        bool windowResizable = false;
    };
}


