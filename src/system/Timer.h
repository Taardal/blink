#pragma once

#include <chrono>

namespace Blink {
    struct Timer {
        std::string tag;
        std::chrono::time_point<std::chrono::steady_clock> start{};
        std::chrono::time_point<std::chrono::steady_clock> end{};
        std::chrono::duration<float> duration{};

        Timer(const std::string& tag = "");

        ~Timer();
    };
}
