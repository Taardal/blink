#pragma once

#include "AppConfig.h"
#include "Keyboard.h"
#include "Window.h"

namespace Blink {
    struct WindowModule {
        Window* window;
        Keyboard* keyboard;

        WindowModule();

        ~WindowModule();

        bool initialize(const AppConfig& appConfig) const;

        void terminate() const;
    };
}
