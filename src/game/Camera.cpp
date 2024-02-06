#include "Camera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Blink {
    Camera::Camera(
        AppConfig appConfig,
        Window* window
    ) : window(window) {
    }

    glm::mat4 Camera::getViewMatrix() const {
        return glm::lookAt(position, position + lookDirection, upDirection);
    }

    glm::mat4 Camera::getProjectionMatrix() const {
        float fieldOfView = glm::radians(45.0f);
        constexpr float nearClip = 0.1f;
        constexpr float farClip = 10.0f;
        return glm::perspective(fieldOfView, aspectRatio, nearClip, farClip);
    }

    bool Camera::initialize() {
        WindowSize windowSize = window->getSizeInPixels();
        aspectRatio = (float) windowSize.width / (float) windowSize.height;
        return true;
    }

    void Camera::onUpdate(double timestep) {
        auto deltaTime = static_cast<float>(timestep);
        GLFWwindow* glfwWindow = window->getGlfwWindow();
        if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
            position += lookDirection * speed * deltaTime;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
            position -= lookDirection * speed * deltaTime;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
            position -= glm::normalize(glm::cross(lookDirection, upDirection)) * speed * deltaTime;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
            position += glm::normalize(glm::cross(lookDirection, upDirection)) * speed * deltaTime;
        }

        glm::vec3 rotate{0};
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
            gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
        }

        BL_LOG_DEBUG("Camera position [{}, {}, {}]", position.x, position.y, position.z);
    }
}
