#pragma once

#include "graphics/Vertex.h"
#include <vector>

namespace Blink {
    struct Model {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };
}