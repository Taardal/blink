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
        return glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
    }

    glm::mat4 Camera::getProjectionMatrix() const {
        float fieldOfView = glm::radians(45.0f);
        WindowSize windowSize = window->getSizeInPixels();
        const float aspectRatio = (float) windowSize.width / (float) windowSize.height;
        constexpr float nearClip = 0.1f;
        constexpr float farClip = 10.0f;
        return glm::perspective(fieldOfView, aspectRatio, nearClip, farClip);
    }

    void Camera::onUpdate(double timestep) {
        GLFWwindow* glfWwindow = window->getGlfwWindow();
        if (glfwGetKey(glfWwindow, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPosition += cameraSpeed * cameraDirection;
        }
        if (glfwGetKey(glfWwindow, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPosition -= cameraSpeed * cameraDirection;
        }
        if (glfwGetKey(glfWwindow, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPosition -= glm::normalize(glm::cross(cameraDirection, cameraUp)) * cameraSpeed;
        }
        if (glfwGetKey(glfWwindow, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPosition += glm::normalize(glm::cross(cameraDirection, cameraUp)) * cameraSpeed;
        }
        BL_LOG_DEBUG("Camera position [{}, {}, {}]", cameraPosition.x, cameraPosition.y, cameraPosition.z);
    }
}
