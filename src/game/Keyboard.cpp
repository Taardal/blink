#include "Keyboard.h"

namespace Blink {
    Keyboard::Keyboard(Window* window) : window(window) {
    }

    bool Keyboard::initialize() const {
        return true;
    }

    void Keyboard::terminate() const {
    }

    bool Keyboard::isPressed(Key key) const {
        return window->isKeyPressed((uint16_t) key);
    }
}
