#include "Camera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Blink {
    Camera::Camera(Window* window) : window(window) {
        updateCameraVectors();
    }

    bool Camera::initialize() {
        WindowSize windowSize = window->getSizeInPixels();
        aspectRatio = (float) windowSize.width / (float) windowSize.height;
        return true;
    }

    glm::mat4 Camera::getViewMatrix() const {
        // Create rotation matrix for the roll angle
        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), frontDirection);

        // Rotate the up vector using the roll matrix
        glm::vec3 rotatedUpDirection = glm::vec3(rollMatrix * glm::vec4(upDirection, 0.0f));

        return glm::lookAt(position, position + frontDirection, rotatedUpDirection);
    }

    glm::mat4 Camera::getProjectionMatrix() const {
        float fieldOfView = glm::radians(45.0f);
        constexpr float nearClip = 0.1f;
        constexpr float farClip = 10.0f;
        return glm::perspective(fieldOfView, aspectRatio, nearClip, farClip);
    }

    void Camera::onUpdate(double timestep) {
        processKeyboardInput((float) timestep);
        updateCameraVectors();
        BL_LOG_DEBUG("position [{}, {}, {}]", position.x, position.y, position.z);
    }

    void Camera::processKeyboardInput(float deltaTime) {
        GLFWwindow* glfwWindow = window->getGlfwWindow();

        float velocity = moveSpeed * deltaTime;
        if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
            position += frontDirection * velocity;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
            position -= frontDirection * velocity;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
            position -= rightDirection * velocity;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
            position += rightDirection * velocity;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            position += worldUpDirection * velocity;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_C) == GLFW_PRESS) {
            position -= worldUpDirection * velocity;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_UP) == GLFW_PRESS) {
            pitch += lookSpeed;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
            pitch -= lookSpeed;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
            yaw -= lookSpeed;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            yaw += lookSpeed;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_Q) == GLFW_PRESS) {
            roll += lookSpeed;
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_E) == GLFW_PRESS) {
            roll -= lookSpeed;
        }
        // Clamp pitch to prevent camera flipping
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }
    }

    void Camera::updateCameraVectors() {
        // Calculate the new front vector
        frontDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        frontDirection.y = sin(glm::radians(pitch));
        frontDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        frontDirection = glm::normalize(frontDirection);

        // Recalculate the right and up vectors
        rightDirection = glm::normalize(glm::cross(frontDirection, worldUpDirection));
        upDirection = glm::normalize(glm::cross(rightDirection, frontDirection));
    }
}
