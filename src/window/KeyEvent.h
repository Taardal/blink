#pragma once

#include "Event.h"
#include "Keyboard.h"

namespace Blink {
    struct KeyEvent : Event {
    public:
        Key key;
        int32_t keyCode;
        int32_t mods;
        int32_t scanCode;

        KeyEvent(EventType type, int32_t keyCode, int32_t mods, int32_t scanCode);

        std::string toString() const override;
    };

    struct KeyPressedEvent final : KeyEvent {
        explicit KeyPressedEvent(int32_t key, int32_t mods, int32_t scanCode);
    };

    struct KeyReleasedEvent final : KeyEvent {
        explicit KeyReleasedEvent(int32_t key, int32_t mods, int32_t scanCode);
    };

    struct KeyRepeatedEvent final : KeyEvent {
        explicit KeyRepeatedEvent(int32_t key, int32_t mods, int32_t scanCode);
    };

    struct KeyTypedEvent final : KeyEvent {
        explicit KeyTypedEvent(int32_t key, int32_t mods, int32_t scanCode);
    };
}
