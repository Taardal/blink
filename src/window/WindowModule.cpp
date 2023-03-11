#include "WindowModule.h"

namespace Blink {
    WindowModule::WindowModule()
            : window(new Window()) {}

    WindowModule::~WindowModule() {
        delete window;
    }

    Window* WindowModule::getWindow() const {
        return window;
    }

    bool WindowModule::initialize(const AppConfig& appConfig) const {
        if (!window->initialize(appConfig)) {
            BL_LOG_ERROR("Could not initialize window");
            return false;
        }
        return true;
    }

    void WindowModule::terminate() const {
        window->terminate();
    }
}
