#pragma once

#include "Config.h"
#include "Window.h"

namespace Blink {
    class WindowModule {
    private:
        Window* window;

    public:
        WindowModule();

        ~WindowModule();

        Window* getWindow() const;

        bool initialize(const Config& config) const;

        void terminate() const;
    };
}
