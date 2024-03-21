#include "WindowEvent.h"
#include <sstream>

namespace Blink {
    WindowCloseEvent::WindowCloseEvent()
        : Event(EventType::WindowClose) {
    }

    WindowMinimizeEvent::WindowMinimizeEvent(
        bool minimized
    ) : Event(EventType::WindowMinimize),
        minimized(minimized) {
    }

    std::string WindowMinimizeEvent::toString() const {
        std::stringstream ss;
        ss << name << "{minimized=" << minimized << "}";
        return ss.str();
    }

    WindowResizeEvent::WindowResizeEvent(
        int32_t width,
        int32_t height
    ) : Event(EventType::WindowResize),
        width(width),
        height(height) {
    }

    std::string WindowResizeEvent::toString() const {
        std::stringstream ss;
        ss << name << "{width=" << width << ", height=" << height << "}";
        return ss.str();
    }
}
