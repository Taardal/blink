#pragma once

#include <tiny_obj_loader.h>

namespace Blink {
    struct ObjFile {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
    };
}
