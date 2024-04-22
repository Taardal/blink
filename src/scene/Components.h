#pragma once

#include "graphics/Vertex.h"
#include "graphics/Mesh.h"

#include <glm/glm.hpp>
#include <string>

namespace Blink {
    struct TransformComponent {
        glm::vec3 position = {0, 0, 0};
        glm::mat4 translation = glm::mat4(1.0f);
        glm::mat4 rotation = glm::mat4(1.0f);
        glm::mat4 scale = glm::mat4(1.0f);
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
