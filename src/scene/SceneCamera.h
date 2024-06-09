#pragma once

#include "window/Window.h"
#include "window/Keyboard.h"
#include "window/Mouse.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Blink {
    struct SceneCameraConfig {
        Window* window = nullptr;
        Keyboard* keyboard = nullptr;
        Mouse* mouse = nullptr;
    };

    class SceneCamera {
        friend class Scene;
        friend class SceneCameraLuaBinding;

    private:
        SceneCameraConfig config;
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 forwardDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 rightDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 upDirection = {0.0f, 0.0f, 0.0f};
        glm::vec3 worldUpDirection = {0.0f, 0.0f, 0.0f};
        glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        float yaw = 0.0f;
        float pitch = 0.0f;
        float roll = 0.0f;
        float moveSpeed = 0.0f;
        float rotationSpeed = 0.0f;
        float fieldOfView = 0.0f;
        float nearClip = 0.0f;
        float farClip = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
        float aspectRatio = 0.0f;
        bool loggingEnabled = false;

    public:
        explicit SceneCamera(const SceneCameraConfig& config);

        void onResize();

        void update(double timestep);

    private:
        void processInput(double timestep);

        void calculateDirections();

        void calculateView();

        void calculateProjection();

        void calculateAspectRatio();

        void logState() const;
    };
}
