#pragma once

#include "system/ImageFile.h"
#include "system/ObjFile.h"

#include <string>

namespace Blink {
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
