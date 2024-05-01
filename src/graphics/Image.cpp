#include "Image.h"

#include "stb_image.h"

namespace Blink {
    Image::~Image() {
        free();
    }

    void Image::free() {
        // if (pixels == nullptr) {
        //     return;
        // }
        // stbi_image_free(pixels);
        // pixels = nullptr;
    }
}