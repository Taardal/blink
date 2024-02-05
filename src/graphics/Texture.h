#pragma once

#include <numbers>

namespace Blink {
    class Texture {
    private:
        uint32_t id = 0;

    public:
        bool initialize();

        void terminate() const;
    };
}
