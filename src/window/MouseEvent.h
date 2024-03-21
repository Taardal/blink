#pragma once

#include "Event.h"

namespace Blink {
    struct MouseMovedEvent final : Event {
        float x;
        float y;

        MouseMovedEvent(float x, float y);

        std::string toString() const override;
    };

    struct MouseScrolledEvent final : Event {
        float xOffset;
        float yOffset;

        MouseScrolledEvent(float xOffset, float yOffset);

        std::string toString() const override;
    };

    struct MouseButtonEvent : Event {
    public:
        int32_t button;

    protected:
        explicit MouseButtonEvent(EventType type, int32_t button);

    public:
        std::string toString() const override;
    };

    struct MouseButtonPressedEvent final : MouseButtonEvent {
        explicit MouseButtonPressedEvent(int32_t button);
    };

    struct MouseButtonReleasedEvent final : MouseButtonEvent {
        explicit MouseButtonReleasedEvent(int32_t button);
    };
}
