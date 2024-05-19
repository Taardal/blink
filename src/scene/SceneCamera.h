#pragma once

#include "window/Window.h"
#include "window/Keyboard.h"
#include "window/Mouse.h"

#include <glm/glm.hpp>

namespace Blink {
    struct EulerAngles {
        float yaw = 0.0f;
        float pitch = 0.0f;
        float roll = 0.0f;
    };

    struct Directions {
        glm::vec3 forwardDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 rightDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 upDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 worldUpDirection = {0.0f, 0.0f, 0.0f};
    };

    struct SceneCameraTransform {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 forwardDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 rightDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 upDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 worldUpDirection = {0.0f, 0.0f, 0.0f};
        float yaw = 0.0f;
        float pitch = 0.0f;
        float roll = 0.0f;
    };

    struct Frustum {
        float fieldOfView = 0.0f;
        float nearClip = 0.0f;
        float farClip = 0.0f;
    };

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
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 forwardDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 rightDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 upDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 worldUpDirection = {0.0f, 0.0f, 0.0f};
        float aspectRatio = 0.0f;
        float yaw = 0.0f;
        float pitch = 0.0f;
        float roll = 0.0f;
        float moveSpeed = 0.0f;
        float rotationSpeed = 0.0f;
        float fieldOfView = 0.0f;
        float nearClip = 0.0f;
        float farClip = 0.0f;

        glm::vec2 mousePosition = {0.0f, 0.0f};
        glm::vec2 lastMousePosition = {0.0f, 0.0f};
        float mouseSensitivity = 0.0f;

        bool inputEnabled = true;
        bool loggingEnabled = false;

        Directions directions{};
        EulerAngles eulerAngles{};
        Frustum frustum{};

    public:
        explicit SceneCamera(const SceneCameraConfig& config);

        glm::mat4 getView() const;

        glm::mat4 getProjection() const;

        void onEvent(Event& event);

        void update(double timestep);

        void logState() const;

        void setMoveSpeed(float moveSpeed);

        void setRotationSpeed(float rotationSpeed);

        void setDirections(const Directions& directions);

        void setEulerAngles(const EulerAngles& eulerAngles);

        void setFrustum(const Frustum& frustum);

        void setPosition(const glm::vec3& position);

        void setForwardDirection(const glm::vec3& forwardDirection);

        void setRightDirection(const glm::vec3& rightDirection);

        void setUpDirection(const glm::vec3& upDirection);

        void setWorldUpDirection(const glm::vec3& worldUpDirection);

    private:
        void calculateAspectRatio();

        void calculateDirections();

        void calculateEulerAngles();

    };
}
