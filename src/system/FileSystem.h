#pragma once

#include <string>

namespace Blink {
    struct Image {
        int32_t width;
        int32_t height;
        int32_t channels;
        uint64_t size;
        unsigned char* pixels = nullptr;

    private:
        Image() = default;

    public:
        ~Image();

        void free();
    };
}

namespace Blink {
    class FileSystem {
    public:
        bool exists(const char* path) const;

        std::vector<char> readBytes(const char* path) const noexcept(false);

        Image readImage(const char* path) const noexcept(false);
    };
}
