#include "pch.h"
#include "FileSystem.h"

#include <filesystem>
#include <fstream>

namespace Blink {
    bool FileSystem::exists(const char* path) const {
        return std::filesystem::exists(path);
    }

    std::vector<char> FileSystem::readBytes(const char* path) const {
        if (!exists(path)) {
            BL_THROW("Could not find file [" + std::string(path) + "]");
        }
        std::ifstream file{path, std::ios::ate | std::ios::binary};
        if (!file.is_open()) {
            BL_THROW("Could not open file with path [" + std::string(path) + "]");
        }
        auto fileSize = (uint32_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }
}
