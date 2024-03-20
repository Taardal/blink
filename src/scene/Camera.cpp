#include "pch.h"
#include "scene/Camera.h"

namespace Blink {
    Camera::Camera(Window* window, Keyboard* keyboard) : window(window), keyboard(keyboard) {
        WindowSize windowSize = window->getSizeInPixels();
        aspectRatio = (float) windowSize.width / (float) windowSize.height;
        updateDirections();
    }

    glm::mat4 Camera::getViewMatrix() const {
        return glm::lookAt(position, position + frontDirection, upDirection);
    }

    glm::mat4 Camera::getProjectionMatrix() const {
        return glm::perspective(fieldOfView, aspectRatio, nearClip, farClip);
    }

    void Camera::update(double timestep) {
        processKeyboardInput((float) timestep);
        updateDirections();
        //logState();
    }

    void Camera::processKeyboardInput(float timestep) {
        float velocity = moveSpeed * timestep;
        if (keyboard->isPressed(Key::W)) {
            position += frontDirection * velocity;
        }
        if (keyboard->isPressed(Key::S)) {
            position -= frontDirection * velocity;
        }
        if (keyboard->isPressed(Key::A)) {
            position -= rightDirection * velocity;
        }
        if (keyboard->isPressed(Key::D)) {
            position += rightDirection * velocity;
        }
        if (keyboard->isPressed(Key::Space)) {
            position += worldUpDirection * velocity;
        }
        if (keyboard->isPressed(Key::C)) {
            position -= worldUpDirection * velocity;
        }

        if (keyboard->isPressed(Key::Up)) {
            pitch += lookSpeed;
        }
        if (keyboard->isPressed(Key::Down)) {
            pitch -= lookSpeed;
        }
        if (keyboard->isPressed(Key::Left)) {
            yaw -= lookSpeed;
        }
        if (keyboard->isPressed(Key::Right)) {
            yaw += lookSpeed;
        }
        if (keyboard->isPressed(Key::Q)) {
            roll += lookSpeed;
        }
        if (keyboard->isPressed(Key::E)) {
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

    void Camera::updateDirections() {
        // Calculate the new front direction vector
        frontDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        frontDirection.y = sin(glm::radians(pitch));
        frontDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        frontDirection = glm::normalize(frontDirection);

        // Recalculate the right and up direction vectors
        rightDirection = glm::normalize(glm::cross(frontDirection, worldUpDirection));
        upDirection = glm::normalize(glm::cross(rightDirection, frontDirection));

        // Rotate the up direction vector using roll
        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), frontDirection);
        upDirection = glm::vec3(rollMatrix * glm::vec4(upDirection, 0.0f));
    }

    void Camera::logState() const {
        BL_LOG_DEBUG("=============== C A M E R A ===============");
        BL_LOG_DEBUG("--");
        BL_LOG_DEBUG("-- Position: [{}, {}, {}]", position.x, position.y, position.z);
        BL_LOG_DEBUG("--");
        BL_LOG_DEBUG("-- Front direction: [{}, {}. {}]", frontDirection.x, frontDirection.y, frontDirection.z);
        BL_LOG_DEBUG("-- Up direction: [{}, {}. {}]", upDirection.x, upDirection.y, upDirection.z);
        BL_LOG_DEBUG("-- Right direction: [{}, {}. {}]", rightDirection.x, rightDirection.y, rightDirection.z);
        BL_LOG_DEBUG("--");
        BL_LOG_DEBUG("-- Yaw: {}°", yaw);
        BL_LOG_DEBUG("-- Pitch: {}°", pitch);
        BL_LOG_DEBUG("-- Roll: {}°", roll);
        BL_LOG_DEBUG("--");
    }
}
