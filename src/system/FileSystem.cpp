#include "FileSystem.h"
#include "Log.h"
#include <fstream>
#include <filesystem>

namespace Blink {
    std::vector<char> FileSystem::readBytes(const char* path) const {
        if (!exists(path)) {
            BL_LOG_ERROR("Could not find file [{}]", path);
            return {};
        }
        std::ifstream file{path, std::ios::ate | std::ios::binary};
        if (!file.is_open()) {
            BL_LOG_ERROR("Could not open file with path [{0}]", path);
            return {};
        }
        uint32_t fileSize = (uint32_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }

    bool FileSystem::exists(const char* string) const {
        return std::filesystem::exists(string);
    }
}
