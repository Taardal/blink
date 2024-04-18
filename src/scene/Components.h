#pragma once

#include "graphics/Vertex.h"
#include "graphics/Model.h"

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

    struct ModelComponent {
        Model model;
    };
}
