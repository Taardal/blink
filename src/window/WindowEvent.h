#pragma once

#include "Event.h"

namespace Blink {
    struct WindowCloseEvent final : Event {
        WindowCloseEvent();
    };

    struct WindowMinimizeEvent final : Event {
        bool minimized;

        explicit WindowMinimizeEvent(bool minimized);

        std::string toString() const override;
    };

    struct WindowResizeEvent final : Event {
        int32_t width;
        int32_t height;

        WindowResizeEvent(int32_t width, int32_t height);

        std::string toString() const override;
    };
}
