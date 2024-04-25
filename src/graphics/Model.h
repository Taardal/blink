#pragma once

#include "graphics/Vertex.h"

#include <tiny_obj_loader.h>
#include <vector>

namespace Blink {
    struct Model {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
    };
}