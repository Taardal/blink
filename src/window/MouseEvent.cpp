#include "MouseEvent.h"
#include <sstream>

namespace Blink {
    MouseMovedEvent::MouseMovedEvent(float x, float y) : Event(EventType::MouseMoved), x(x), y(y) {
    }

    std::string MouseMovedEvent::toString() const {
        std::stringstream ss;
        ss << name << "{x=" << x << ", y=" << y << "}";
        return ss.str();
    }

    std::ostream& operator<<(std::ostream& os, const MouseMovedEvent& event) {
        os << event.toString();
        return os;
    }
}

namespace Blink {
    MouseScrolledEvent::MouseScrolledEvent(float xOffset, float yOffset)
        : Event(EventType::MouseScrolled), xOffset(xOffset), yOffset(yOffset) {
    }

    std::string MouseScrolledEvent::toString() const {
        std::stringstream ss;
        ss << name << "{xOffset=" << xOffset << ", yOffset=" << yOffset << "}";
        return ss.str();
    }

    std::ostream& operator<<(std::ostream& os, const MouseScrolledEvent& event) {
        os << event.toString();
        return os;
    }
}

namespace Blink {
    MouseButtonEvent::MouseButtonEvent(EventType type, int32_t button) : Event(type), button(button) {
    }

    std::string MouseButtonEvent::toString() const {
        std::stringstream ss;
        ss << name << "{button=" << button << "}";
        return ss.str();
    }

    std::ostream& operator<<(std::ostream& os, const MouseButtonEvent& event) {
        os << event.toString();
        return os;
    }
}

namespace Blink {
    MouseButtonPressedEvent::MouseButtonPressedEvent(int32_t button)
        : MouseButtonEvent(EventType::MouseButtonPressed, button) {
    }
}

namespace Blink {
    MouseButtonReleasedEvent::MouseButtonReleasedEvent(int32_t button)
        : MouseButtonEvent(EventType::MouseButtonReleased, button) {
    }
}
