#pragma once

#include <tiny_obj_loader.h>
#include <string>

namespace Blink {

    struct ImageFile {
        int32_t width;
        int32_t height;
        int32_t channels;
        uint64_t size;
        unsigned char* pixels = nullptr;

        ImageFile() = default;

        ~ImageFile();

        void free();
    };

    struct ObjFile {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
    };

    class FileSystem {
    public:
        bool exists(const std::string& path) const;

        std::vector<char> readBytes(const std::string& path) const;

        std::shared_ptr<ImageFile> readImage(const std::string& path) const;

        std::shared_ptr<ObjFile> readObj(const std::string& path) const;

    private:
        void cleanPath(std::string* path) const;
    };
}
