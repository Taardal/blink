#pragma once

#include "Window.h"
#include "GraphicsContext.h"
#include "Log.h"
#include <string>

namespace blink
{
    struct Config
    {
        Window::Config WindowConfig;
        GraphicsContext::Config GraphicsConfig;
        LogLevel LogLevel = LogLevel::None;
    };
}


