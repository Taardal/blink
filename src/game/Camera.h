#pragma once

#include "window/Window.h"

#include <glm/glm.hpp>

namespace Blink {
    class Camera {
    private:
        Window* window;
        glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 2.0f);
        glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        float cameraAngleInDegrees = 0.0f;
        float cameraSpeed = 0.05f;

    public:
        Camera(AppConfig appConfig, Window* window);

        glm::mat4 getViewMatrix() const;

        glm::mat4 getProjectionMatrix() const;

        void onUpdate(double timestep);
    };
}
