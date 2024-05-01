#include "ImageFile.h"

#include "stb_image.h"

namespace Blink {
    ImageFile::~ImageFile() {
        free();
    }

    void ImageFile::free() {
        if (pixels == nullptr) {
            return;
        }
        stbi_image_free(pixels);
        pixels = nullptr;
    }
}