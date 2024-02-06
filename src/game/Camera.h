#pragma once

#include "window/Window.h"

#include <glm/glm.hpp>

namespace Blink {
    class Camera {
    private:
        Window* window;
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 frontDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 upDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 worldUpDirection = {0.0f, 1.0f, 0.0f};
        glm::vec3 rightDirection = {0.0f, 0.0f, 0.0f};
        float aspectRatio = 0.0f;
        float yaw = -90.0f;
        float pitch = 0.0f;
        float roll = 0.0f;
        float moveSpeed = 2.5f;
        float lookSpeed = 1.0f;
        float fieldOfView = glm::radians(45.0f);
        float nearClip = 0.1f;
        float farClip = 10.0f;

    public:
        explicit Camera(Window* window);

        glm::mat4 getViewMatrix() const;

        glm::mat4 getProjectionMatrix() const;

        bool initialize();

        void onUpdate(double timestep);

        void processKeyboardInput(float timestep);

        void updateDirections();

        void logState() const;
    };
}
