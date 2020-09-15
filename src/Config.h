#pragma once

#include "Window.h"
#include "GraphicsContext.h"
#include "Log.h"
#include <string>

namespace blink
{
    struct Config
    {
        LogLevel LogLevel = LogLevel::None;
        Window::Config WindowConfig;
        GraphicsContext::Config GraphicsConfig;
    };
}


