#include "WindowModule.h"

namespace Blink {
    WindowModule::WindowModule()
        : window(new Window()), keyboard(new Keyboard(window)) {
    }

    WindowModule::~WindowModule() {
        delete keyboard;
        delete window;
    }

    bool WindowModule::initialize(const AppConfig& appConfig) const {
        if (!window->initialize(appConfig)) {
            BL_LOG_ERROR("Could not initialize window");
            return false;
        }
        if (!keyboard->initialize()) {
            BL_LOG_ERROR("Could not initialize keyboard");
            return false;
        }
        return true;
    }

    void WindowModule::terminate() const {
        keyboard->terminate();
        window->terminate();
    }
}
