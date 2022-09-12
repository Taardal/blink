#include "FileSystem.h"
#include "Log.h"
#include <fstream>
#include <filesystem>

namespace Blink {
    std::string FileSystem::readFile(const char* path) const {
        if (!exists(path)) {
            BL_LOG_ERROR("Could not find file [{}]", path);
            return "";
        }
        std::string result;
        std::ifstream inputStream(path, std::ios::in | std::ios::binary);
        if (inputStream) {
            inputStream.seekg(0, std::ios::end);
            size_t length = inputStream.tellg();
            if (length != -1) {
                result.resize(length);
                inputStream.seekg(0, std::ios::beg);
                inputStream.read(&result[0], (std::streamsize) length);
                inputStream.close();
            }
        }
        return result;
    }

    bool FileSystem::exists(const char* string) const {
        return std::filesystem::exists(string);
    }
}
