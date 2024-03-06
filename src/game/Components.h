#pragma once

#include <glm/glm.hpp>
#include <string>

#include "system/Uuid.h"

namespace Blink {
    struct TransformComponent {
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    };

    struct IdComponent {
        std::string id;

        IdComponent() : id(Uuid::create()) {
        }
    };

    struct ScriptComponent {
        std::string type;

        ScriptComponent(const std::string& type) : type(type) {
        }
    };
}
