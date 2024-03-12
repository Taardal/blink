#pragma once

#include "system/Uuid.h"

#include <glm/glm.hpp>
#include <string>

namespace Blink {
    struct TransformComponent {
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    };

    struct IdComponent {
        std::string id;

        IdComponent() : id(Uuid::create()) {
        }
    };

    struct LuaComponent {
        std::string filepath;
        std::string luaTableName;
    };
}
