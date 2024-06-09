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

    std::ostream& operator<<(std::ostream& os, const KeyEvent& event);
}

namespace Blink {

    #define BL_IS_KEY_PRESSED_EVENT(KEY) event.type == EventType::KeyPressed && event.as<KeyPressedEvent>().key == KEY

    struct KeyPressedEvent final : KeyEvent {
        explicit KeyPressedEvent(int32_t key, int32_t mods, int32_t scanCode);
    };
}

namespace Blink {
    struct KeyReleasedEvent final : KeyEvent {
        explicit KeyReleasedEvent(int32_t key, int32_t mods, int32_t scanCode);
    };
}

namespace Blink {
    struct KeyRepeatedEvent final : KeyEvent {
        explicit KeyRepeatedEvent(int32_t key, int32_t mods, int32_t scanCode);
    };
}

namespace Blink {
    struct KeyTypedEvent final : KeyEvent {
        explicit KeyTypedEvent(int32_t key, int32_t mods, int32_t scanCode);
    };
}
