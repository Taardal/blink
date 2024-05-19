#pragma once

#include "graphics/Mesh.h"
#include "graphics/MeshManager.h"

#include <glm/glm.hpp>
#include <string>

namespace Blink {
    struct TransformComponent {
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 translation = glm::mat4(1.0f);
        glm::mat4 rotation = glm::mat4(1.0f);
        glm::mat4 scale = glm::mat4(1.0f);

        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 size = {1.0f, 1.0f, 1.0f};

        glm::vec3 forwardDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 rightDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 upDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 worldUpDirection = {0.0f, 0.0f, 0.0f};

        float yaw = 0.0f;
        float pitch = 0.0f;
        float roll = 0.0f;

        // Models may different coordinate systems in model space and must be offset during rendering to make them
        // face the correct way and be consistent with the transform
        float yawModelSpaceOffset = 0.0f;
        float pitchModelSpaceOffset = 0.0f;
        float rollModelSpaceOffset = 0.0f;
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

    struct FighterJetComponent {
        std::string type;
    };
}
