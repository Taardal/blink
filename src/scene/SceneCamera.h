#pragma once

#include "window/Window.h"
#include "window/Keyboard.h"
#include "window/Mouse.h"

#include <glm/glm.hpp>

namespace Blink {
    struct SceneCameraConfig {
        Window* window = nullptr;
        Keyboard* keyboard = nullptr;
        Mouse* mouse = nullptr;
    };

    class SceneCamera {
    public:
        SceneCameraConfig config;
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::vec3 position = {0, 0, 0};
        glm::vec3 forwardDirection = {0, 0, 0};
        glm::vec3 rightDirection = {0, 0, 0};
        glm::vec3 upDirection = {0, 0, 0};
        glm::vec3 worldUpDirection = {0, 0, 0};
        float aspectRatio = 0.0f;
        float yaw = 0.0f;
        float pitch = 0.0f;
        float roll = 0.0f;
        float moveSpeed = 10.0;
        float rotationSpeed = 1.0f;
        float fieldOfView = glm::radians(45.0f);
        float nearClip = 0.1f;
        float farClip = 10000.0f;

        glm::vec2 mousePosition = {0.0f, 0.0f};
        glm::vec2 lastMousePosition = {0.0f, 0.0f};
        float mouseSensitivity = 0.0;

    public:
        explicit SceneCamera(const SceneCameraConfig& config);

        glm::mat4 getView() const;

        glm::mat4 getProjection() const;

        glm::mat4 getRotation() const;

        void setPosition(const glm::vec3& position);

        void setForwardDirection(const glm::vec3& forwardDirection);

        void onEvent(Event& event);

        void update(double timestep);

    private:
        void processKeyboardInput(float timestep);

        void updateDirections();

        void updateAspectRatio();

        void logState() const;
    };
}
