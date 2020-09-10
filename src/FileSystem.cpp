#include "FileSystem.h"
#include <fstream>
#include <filesystem>

namespace blink
{
    std::string FileSystem::ReadFile(const char* path) const
    {
        std::string result;
        std::ifstream inputStream(path, std::ios::in | std::ios::binary);
        if (inputStream)
        {
            inputStream.seekg(0, std::ios::end);
            size_t length = inputStream.tellg();
            if (length != -1)
            {
                result.resize(length);
                inputStream.seekg(0, std::ios::beg);
                inputStream.read(&result[0], length);
                inputStream.close();
            }
        }
        return result;
    }

    bool FileSystem::Exists(const char* string) const
    {
        return std::filesystem::exists(string);
    }
}
