#include "FileSystem.h"
#include "Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>
#include <fstream>

namespace Blink {
    Image::~Image() {
        free();
    }

    void Image::free() {
        if (pixels == nullptr) {
            return;
        }
        stbi_image_free(pixels);
        pixels = nullptr;
    }
}

namespace Blink {
    bool FileSystem::exists(const char* path) const {
        return std::filesystem::exists(path);
    }

    std::vector<char> FileSystem::readBytes(const char* path) const noexcept(false) {
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

    Image FileSystem::readImage(const char* path) const noexcept(false) {
        int32_t width;
        int32_t height;
        int32_t channels;
        int32_t desiredChannels = STBI_rgb_alpha;
        unsigned char* pixels = stbi_load(path, &width, &height, &channels, desiredChannels);
        if (!pixels) {
            BL_THROW("Could not read image [" + std::string(path) + "]");
        }
        Image image{};
        image.width = width;
        image.height = height;
        image.channels = channels;
        image.size = width * height * channels;
        image.pixels = pixels;
        return image;
    }
}
