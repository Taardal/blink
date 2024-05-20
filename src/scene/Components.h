#pragma once

#include "graphics/Mesh.h"
#include "graphics/MeshManager.h"

#include <glm/glm.hpp>
#include <string>

namespace Blink {
    struct TransformComponent {
        glm::mat4 translation = glm::mat4(1.0f);
        glm::mat4 rotation = glm::mat4(1.0f);
        glm::mat4 scale = glm::mat4(1.0f);

        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 size = {1.0f, 1.0f, 1.0f};

        glm::vec3 forwardDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 rightDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 upDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 worldUpDirection = {0.0f, 0.0f, 0.0f};

        glm::quat orientation{};
        glm::quat orientationOffset{};

        float yaw = 0.0f;
        float pitch = 0.0f;
        float roll = 0.0f;

        float yawOffset = 0.0f;
        float pitchOffset = 0.0f;
        float rollOffset = 0.0f;
    };

    struct TagComponent {
        std::string tag;
    };

    struct LuaComponent {
        std::string type;
        std::string path;
    };

    struct MeshComponent {
        std::shared_ptr<Mesh> mesh;
        MeshInfo meshInfo;
    };

    struct CameraComponent {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
    };
}
