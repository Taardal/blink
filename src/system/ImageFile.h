#pragma once

namespace Blink {
    struct ImageFile {
        int32_t width;
        int32_t height;
        int32_t channels;
        uint64_t size;
        unsigned char* pixels = nullptr;

        ImageFile() = default;

        ~ImageFile();

        void free();
    };
}
