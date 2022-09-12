#pragma once

#include <string>

namespace Blink
{
    class FileSystem
    {
    public:
        std::string readFile(const char* path) const;

        bool exists(const char* string) const;
    };
}
