#include "Keyboard.h"

namespace Blink {
    Keyboard::Keyboard(Window* window) : window(window) {
    }

    bool Keyboard::initialize() {
        return true;
    }

    void Keyboard::terminate() const {
    }

    bool Keyboard::isPressed(Key key) const {
        return window->isKeyPressed((uint16_t) key);
    }

    bool Keyboard::isPressed(uint16_t key) const {
        return window->isKeyPressed(key);
    }

    bool Keyboard::isPressed(const std::string& keyName) const {
        Key key = keysByName.at(keyName);
        return window->isKeyPressed((uint16_t) key);
    }
}
