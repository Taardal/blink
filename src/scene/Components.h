#pragma once

#include "graphics/Mesh.h"

#include <glm/glm.hpp>
#include <string>

namespace Blink {
    struct TransformComponent {
        glm::mat4 translation = glm::mat4(1.0f);
        glm::mat4 rotation = glm::mat4(1.0f);
        glm::mat4 scale = glm::mat4(1.0f);
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 forwardDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 rightDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 upDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 worldUpDirection = {0.0f, 0.0f, 0.0f};
        float yaw = 0.0f;
        float pitch = 0.0f;
        float roll = 0.0f;

        float yawOffset = 0.0f;
        float pitchOffset = 0.0f;
        float rollOffset = 0.0f;

        glm::mat4 getRotation() const {
            glm::mat4 rotationMatrix(1.0f);
            rotationMatrix[0] = glm::vec4(rightDirection, 0.0f);
            rotationMatrix[1] = glm::vec4(upDirection, 0.0f);
            rotationMatrix[2] = glm::vec4(-forwardDirection, 0.0f);
            return glm::transpose(rotationMatrix);
        }

        glm::mat4 getRotation1() const {
            glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 rotationMatrix = yawMatrix * pitchMatrix * rollMatrix;
            return glm::transpose(rotationMatrix);
        }

        void logState() const {
            BL_LOG_DEBUG("===============[ E N T I T Y ]===============");
            BL_LOG_DEBUG("--");
            BL_LOG_DEBUG("-- Position: [{}, {}, {}]", position.x, position.y, position.z);
            BL_LOG_DEBUG("--");
            BL_LOG_DEBUG("-- Forward direction: [{}, {}, {}]", forwardDirection.x, forwardDirection.y, forwardDirection.z);
            BL_LOG_DEBUG("-- Up direction: [{}, {}, {}]", upDirection.x, upDirection.y, upDirection.z);
            BL_LOG_DEBUG("-- Right direction: [{}, {}, {}]", rightDirection.x, rightDirection.y, rightDirection.z);
            BL_LOG_DEBUG("--");
            BL_LOG_DEBUG("-- Yaw: {}°", yaw);
            BL_LOG_DEBUG("-- Pitch: {}°", pitch);
            BL_LOG_DEBUG("-- Roll: {}°", roll);
            BL_LOG_DEBUG("--");
        }
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
    };

    struct CameraComponent {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
    };
}
