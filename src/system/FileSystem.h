#pragma once

#include <string>

namespace Blink {
    class FileSystem {
    public:
        std::vector<char> readBytes(const char* path) const;

        bool exists(const char* string) const;
    };
}
