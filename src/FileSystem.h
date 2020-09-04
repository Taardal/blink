#pragma once

#include <string>

namespace blink
{
    class FileSystem
    {
    public:
        std::string ReadFile(const char* path) const;
    };
}


