#pragma once

#include "Event.h"

namespace Blink {
    struct MouseMovedEvent final : Event {
        float x;
        float y;

        MouseMovedEvent(float x, float y);

        std::string toString() const override;
    };

    std::ostream& operator<<(std::ostream& os, const MouseMovedEvent& event);
}

namespace Blink {
    struct MouseScrolledEvent final : Event {
        float xOffset;
        float yOffset;

        MouseScrolledEvent(float xOffset, float yOffset);

        std::string toString() const override;
    };

    std::ostream& operator<<(std::ostream& os, const MouseScrolledEvent& event);
}

namespace Blink {
    struct MouseButtonEvent : Event {
    public:
        int32_t button;

    protected:
        explicit MouseButtonEvent(EventType type, int32_t button);

    public:
        std::string toString() const override;
    };

    std::ostream& operator<<(std::ostream& os, const MouseButtonEvent& event);
}

namespace Blink {
    struct MouseButtonPressedEvent final : MouseButtonEvent {
        explicit MouseButtonPressedEvent(int32_t button);
    };
}

namespace Blink {
    struct MouseButtonReleasedEvent final : MouseButtonEvent {
        explicit MouseButtonReleasedEvent(int32_t button);
    };
}


