#include "Camera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Blink {
    Camera::Camera(Window* window) : window(window) {
    }

    glm::mat4 Camera::getViewMatrix() const {
        return glm::lookAt(translation, translation + lookDirection, worldUpDirection);
    }

    glm::mat4 Camera::getProjectionMatrix() const {
        float fieldOfView = glm::radians(45.0f);
        constexpr float nearClip = 0.1f;
        constexpr float farClip = 10.0f;
        return glm::perspective(fieldOfView, aspectRatio, nearClip, farClip);
    }

    void Camera::onUpdate(double timestep) {
        auto deltaTime = static_cast<float>(timestep);
        GLFWwindow* glfwWindow = window->getGlfwWindow();

        glm::vec3 rotate{ 0.0f, 0.0f, 0.0f };
        if (glfwGetKey(glfwWindow, GLFW_KEY_UP) == GLFW_PRESS) {
            rotate.x += 1.0f;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
            rotate.x -= 1.0f;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
            rotate.y += 1.0f;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            rotate.y -= 1.0f;
        }
        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
            rotation += lookSpeed * deltaTime * glm::normalize(rotate);
        }
        BL_LOG_DEBUG("rotation [{}, {}, {}]", rotation.x, rotation.y, rotation.z);

        yaw = rotation.y;
        forwardDirection = glm::vec3{ sin(yaw), 0.f, cos(yaw) };
        BL_LOG_DEBUG("forwardDirection [{}, {}, {}]", rotation.x, rotation.y, rotation.z);
        rightDirection = glm::vec3{ forwardDirection.z, 0.f, -forwardDirection.x };
        BL_LOG_DEBUG("rightDirection [{}, {}, {}]", rotation.x, rotation.y, rotation.z);

        glm::vec3 moveDirection{ 0.0f, 0.0f, 0.0f };
        if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
            moveDirection += forwardDirection;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
            moveDirection -= forwardDirection;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
            moveDirection -= rightDirection;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
            moveDirection += rightDirection;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_Q) == GLFW_PRESS) {
            moveDirection += worldUpDirection;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_E) == GLFW_PRESS) {
            moveDirection -= worldUpDirection;
        }
        if (glm::dot(moveDirection, moveDirection) > std::numeric_limits<float>::epsilon()) {
            translation += moveSpeed * deltaTime * glm::normalize(moveDirection);
        }
        BL_LOG_DEBUG("translation [{}, {}, {}]", translation.x, translation.y, translation.z);
    }
}
