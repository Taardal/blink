#include "pch.h"
#include "scene/Camera.h"

namespace Blink {
    Camera::Camera(const CameraConfig& config) : config(config) {
        updateAspectRatio();

        position = {0, 0, 0};
        forwardDirection = {0, 0, -1};
        upDirection = {0, 1, 0};

        updateDirections();
    }

    glm::mat4 Camera::getView() const {
        return view;
    }

    glm::mat4 Camera::getProjection() const {
        return projection;
    }

    void Camera::setPosition(const glm::vec3& position) {
        this->position = position;
    }

    void Camera::onEvent(Event& event) {
        if (event.type == EventType::WindowResize) {
            updateAspectRatio();
        }
    }

    void Camera::update(double timestep) {
        processKeyboardInput((float) timestep);
        updateDirections();
        view = glm::lookAt(position, position + forwardDirection, upDirection);
        projection = glm::perspective(fieldOfView, aspectRatio, nearClip, farClip);
        //logState();
    }

    void Camera::processKeyboardInput(float timestep) {
        float velocity = moveSpeed * timestep;
        if (config.keyboard->isPressed(Key::W)) {
            position += forwardDirection * velocity;
        }
        if (config.keyboard->isPressed(Key::S)) {
            position -= forwardDirection * velocity;
        }
        if (config.keyboard->isPressed(Key::A)) {
            position -= rightDirection * velocity;
        }
        if (config.keyboard->isPressed(Key::D)) {
            position += rightDirection * velocity;
        }
        if (config.keyboard->isPressed(Key::Space)) {
            position += worldUpDirection * velocity;
        }
        if (config.keyboard->isPressed(Key::C)) {
            position -= worldUpDirection * velocity;
        }

        if (config.keyboard->isPressed(Key::Up)) {
            pitch += lookSpeed;
        }
        if (config.keyboard->isPressed(Key::Down)) {
            pitch -= lookSpeed;
        }
        if (config.keyboard->isPressed(Key::Left)) {
            yaw -= lookSpeed;
        }
        if (config.keyboard->isPressed(Key::Right)) {
            yaw += lookSpeed;
        }
        if (config.keyboard->isPressed(Key::Q)) {
            roll += lookSpeed;
        }
        if (config.keyboard->isPressed(Key::E)) {
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
        forwardDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        forwardDirection.y = sin(glm::radians(pitch));
        forwardDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        forwardDirection = glm::normalize(forwardDirection);

        // Recalculate the right and up direction vectors
        rightDirection = glm::normalize(glm::cross(forwardDirection, worldUpDirection));
        upDirection = glm::normalize(glm::cross(rightDirection, forwardDirection));

        // Apply roll rotation
        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), forwardDirection);
        upDirection = glm::vec3(rollMatrix * glm::vec4(upDirection, 0.0f));
        rightDirection = glm::vec3(rollMatrix * glm::vec4(rightDirection, 0.0f));
    }

    void Camera::updateAspectRatio() {
        WindowSize windowSize = config.window->getSizeInPixels();
        aspectRatio = (float) windowSize.width / (float) windowSize.height;
    }

    void Camera::logState() const {
        BL_LOG_DEBUG("=============== C A M E R A ===============");
        BL_LOG_DEBUG("--");
        BL_LOG_DEBUG("-- Position: [{}, {}, {}]", position.x, position.y, position.z);
        BL_LOG_DEBUG("--");
        BL_LOG_DEBUG("-- Front direction: [{}, {}, {}]", forwardDirection.x, forwardDirection.y, forwardDirection.z);
        BL_LOG_DEBUG("-- Up direction: [{}, {}, {}]", upDirection.x, upDirection.y, upDirection.z);
        BL_LOG_DEBUG("-- Right direction: [{}, {}, {}]", rightDirection.x, rightDirection.y, rightDirection.z);
        BL_LOG_DEBUG("--");
        BL_LOG_DEBUG("-- Yaw: {}°", yaw);
        BL_LOG_DEBUG("-- Pitch: {}°", pitch);
        BL_LOG_DEBUG("-- Roll: {}°", roll);
        BL_LOG_DEBUG("--");
    }
}
