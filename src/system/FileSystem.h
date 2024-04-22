#pragma once

#include <string>

namespace Blink {
    class FileSystem {
    public:
        bool exists(const char* path) const;

        std::vector<char> readBytes(const char* path) const;
    };
}
