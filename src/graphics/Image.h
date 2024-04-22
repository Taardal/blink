#pragma once

namespace Blink {
    struct Image {
        int32_t width;
        int32_t height;
        int32_t channels;
        uint64_t size;
        unsigned char* pixels = nullptr;

        Image() = default;

        ~Image();

        void free();
    };
}
