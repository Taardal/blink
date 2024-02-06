#pragma once

#include "window/Window.h"

#include <glm/glm.hpp>

namespace Blink {
    class Camera {
    private:
        Window* window;
        glm::vec3 position = { 0.0f, 0.0f, 2.0f };
        glm::vec3 lookDirection = { 0.0f, 0.0f, -1.0f };
        glm::vec3 worldUpDirection = { 0.0f, 1.0f, 0.0f };
        float angleInDegrees = 0.0f;
        float speed = 1.0f;
        float aspectRatio = 0.0f;

        float lookSpeed = 2.5f;
        float moveSpeed = 2.5f;
        float yaw = 0.0f;
        glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 translation = { 0.0f, 0.0f, 2.0f };
        glm::vec3 forwardDirection{ 0.0f, 0.0f, -1.0f };
        glm::vec3 rightDirection{ 1.0f, 0.0f, 0.0f };

    public:
        Camera(Window* window);

        glm::mat4 getViewMatrix() const;

        glm::mat4 getProjectionMatrix() const;

        bool initialize();

        void onUpdate(double timestep);
    };
}
