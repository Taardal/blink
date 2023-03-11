#pragma once

#include "system/Log.h"
#include <string>
#include <vector>

namespace Blink {

    struct AppConfig {
        LogLevel logLevel = LogLevel::Trace;
        std::string windowTitle = "App";
        int windowWidth = 800;
        int windowHeight = 600;
        bool windowMaximized = false;
        bool windowResizable = false;
    };
}


