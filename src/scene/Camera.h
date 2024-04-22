#pragma once

#include "window/Window.h"
#include "window/Keyboard.h"

#include <glm/glm.hpp>

namespace Blink {
    struct CameraConfig {
        Window* window = nullptr;
        Keyboard* keyboard = nullptr;
    };

    class Camera {
    private:
        CameraConfig config;
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::vec3 position = {0, 0, 0};
        glm::vec3 forwardDirection = {0, 0, -1};
        glm::vec3 rightDirection = {0, 0, 0};
        glm::vec3 upDirection = {0, 1, 0};
        glm::vec3 worldUpDirection = {0, 1, 0};
        float aspectRatio = 0.0f;
        float yaw = 0;
        float pitch = 0.0f;
        float roll = 0.0f;
        float moveSpeed = 2.5f;
        float lookSpeed = 1.0f;
        float fieldOfView = glm::radians(45.0f);
        float nearClip = 0.1f;
        float farClip = 10.0f;

    public:
        explicit Camera(const CameraConfig& config);

        glm::mat4 getView() const;

        glm::mat4 getProjection() const;

        void setPosition(const glm::vec3& position);

        void onEvent(Event& event);

        void update(double timestep);

    private:
        void processKeyboardInput(float timestep);

        void updateDirections();

        void updateAspectRatio();

        void logState() const;
    };
}
