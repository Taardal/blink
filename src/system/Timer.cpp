#include "Timer.h"

namespace Blink {
    Timer::Timer(const std::string& tag) : tag(tag), start(std::chrono::high_resolution_clock::now()) {
    }

    Timer::~Timer() {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        float ms = duration.count() * 1000.0f;

        std::cout << "Timer";
        if (!tag.empty()) {
            std::cout << " ";
            std::cout << "[" << tag << "]";
        }
        std::cout << " ";
        std::cout << "took [" << ms << "] ms";
        std::cout << std::endl;
    }
}
