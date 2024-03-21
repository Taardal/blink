#include "pch.h"
#include "KeyEvent.h"

namespace Blink {
    KeyEvent::KeyEvent(
        EventType type,
        int32_t keyCode,
        int32_t mods,
        int32_t scanCode
    ) : Event(type),
        key((Key) keyCode),
        keyCode(keyCode),
        mods(mods),
        scanCode(scanCode) {
    }

    std::string KeyEvent::toString() const {
        std::stringstream ss;
        ss << name;
        ss << "{";
        ss << "key=" << Keyboard::getKeyName(key);
        ss << ", keyCode=" << keyCode;
        ss << ", mods=" << mods;
        ss << ", scanCode=" << scanCode;
        ss << "}";
        return ss.str();
    }

    KeyPressedEvent::KeyPressedEvent(int32_t key, int32_t mods, int32_t scanCode)
        : KeyEvent(EventType::KeyPressed, key, mods, scanCode) {
    }

    KeyReleasedEvent::KeyReleasedEvent(int32_t key, int32_t mods, int32_t scanCode)
        : KeyEvent(EventType::KeyReleased, key, mods, scanCode) {
    }

    KeyRepeatedEvent::KeyRepeatedEvent(int32_t key, int32_t mods, int32_t scanCode)
        : KeyEvent(EventType::KeyRepeated, key, mods, scanCode) {
    }

    KeyTypedEvent::KeyTypedEvent(int32_t key, int32_t mods, int32_t scanCode)
        : KeyEvent(EventType::KeyTyped, key, mods, scanCode) {
    }
}
