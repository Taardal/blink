#pragma once

#include <random>

namespace Blink {
    class Random {
    private:
        std::random_device randomDevice;
        std::mt19937 engine;

    private:
        Random();

        Random(const Random&) = delete;

        Random& operator=(const Random&) = delete;

        static Random& get();

    public:
        template<typename T = int>
        static T getInt() {
            std::uniform_int_distribution<T> distribution;
            return distribution(get().engine);
        }

        template<typename T = int>
        static T getIntBetween(const uint32_t min, const uint32_t max) {
            std::uniform_int_distribution<T> distribution(min, max);
            return distribution(get().engine);
        }
    };
}
