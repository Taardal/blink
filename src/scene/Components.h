#pragma once

#include "graphics/Vertex.h"
#include "graphics/Mesh.h"

#include <glm/glm.hpp>
#include <string>

namespace Blink {
    struct TransformComponent {
        glm::vec3 position;
    };

    struct TagComponent {
        std::string tag;
    };

    struct LuaComponent {
        std::string type;
        std::string filepath;
    };

    struct MeshComponent {
        Mesh mesh;
    };
}
