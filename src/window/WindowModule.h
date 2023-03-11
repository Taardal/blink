#pragma once

#include "AppConfig.h"
#include "Window.h"

namespace Blink {
    class WindowModule {
    private:
        Window* window;

    public:
        WindowModule();

        ~WindowModule();

        Window* getWindow() const;

        bool initialize(const AppConfig& appConfig) const;

        void terminate() const;
    };
}
