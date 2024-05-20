#include "pch.h"
#include "window/Mouse.h"
#include "scene/SceneCamera.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Blink {
    SceneCamera::SceneCamera(const SceneCameraConfig& config) : config(config) {
        calculateAspectRatio();
    }

    void SceneCamera::onEvent(Event& event) {
        if (event.type == EventType::WindowResize) {
            calculateAspectRatio();
        }
    }

    void SceneCamera::update(double timestep) {
        processInput(timestep);
        calculateDirections();
        calculateView();
        calculateProjection();
        if (loggingEnabled) {
            logState();
        }
    }

    void SceneCamera::processInput(double timestep) {
        float velocity = moveSpeed * timestep;
        if (config.keyboard->isPressed(Key::W)) {
            position += forwardDirection * velocity;
        }
        if (config.keyboard->isPressed(Key::S)) {
            position -= forwardDirection * velocity;
        }
        if (config.keyboard->isPressed(Key::D)) {
            position += rightDirection * velocity;
        }
        if (config.keyboard->isPressed(Key::A)) {
            position -= rightDirection * velocity;
        }
        if (config.keyboard->isPressed(Key::Space)) {
            position += worldUpDirection * velocity;
        }
        if (config.keyboard->isPressed(Key::C)) {
            position -= worldUpDirection * velocity;
        }
        if (config.keyboard->isPressed(Key::Up)) {
            pitch += rotationSpeed;
        }
        if (config.keyboard->isPressed(Key::Down)) {
            pitch -= rotationSpeed;
        }
        if (config.keyboard->isPressed(Key::Left)) {
            yaw -= rotationSpeed;
        }
        if (config.keyboard->isPressed(Key::Right)) {
            yaw += rotationSpeed;
        }
        if (config.keyboard->isPressed(Key::Q)) {
            roll -= rotationSpeed;
        }
        if (config.keyboard->isPressed(Key::E)) {
            roll += rotationSpeed;
        }
        // Clamp pitch to prevent camera flipping
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }
    }

    void SceneCamera::calculateDirections() {
        forwardDirection.x = clampToZero(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
        forwardDirection.y = clampToZero(sin(glm::radians(pitch)));
        forwardDirection.z = clampToZero(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
        forwardDirection = glm::normalize(forwardDirection);

        rightDirection = glm::normalize(glm::cross(forwardDirection, worldUpDirection));
        upDirection = glm::normalize(glm::cross(rightDirection, forwardDirection));

        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), forwardDirection);
        upDirection = glm::vec3(rollMatrix * glm::vec4(upDirection, 0.0f));
        rightDirection = glm::vec3(rollMatrix * glm::vec4(rightDirection, 0.0f));
    }

    void SceneCamera::calculateView() {
        view = glm::lookAt(position, position + forwardDirection, upDirection);
    }

    void SceneCamera::calculateProjection() {
        projection = glm::perspective(fieldOfView, aspectRatio, nearClip, farClip);
    }

    void SceneCamera::calculateAspectRatio() {
        aspectRatio = config.window->getAspectRatio();
    }

    void SceneCamera::logState() const {
        BL_LOG_DEBUG("--------------------------------------------------------------------------------------------------------------");
        BL_LOG_DEBUG("Scene camera");
        BL_LOG_DEBUG("--------------------------------------------------------------------------------------------------------------");
        BL_LOG_DEBUG("Position: [{}, {}, {}]", position.x, position.y, position.z);
        BL_LOG_DEBUG("");
        BL_LOG_DEBUG("Forward direction: [{}, {}, {}]", forwardDirection.x, forwardDirection.y, forwardDirection.z);
        BL_LOG_DEBUG("Up direction: [{}, {}, {}]", upDirection.x, upDirection.y, upDirection.z);
        BL_LOG_DEBUG("Right direction: [{}, {}, {}]", rightDirection.x, rightDirection.y, rightDirection.z);
        BL_LOG_DEBUG("");
        BL_LOG_DEBUG("Yaw: {}°", yaw);
        BL_LOG_DEBUG("Pitch: {}°", pitch);
        BL_LOG_DEBUG("Roll: {}°", roll);
        BL_LOG_DEBUG("");
        BL_LOG_DEBUG("--------------------------------------------------------------------------------------------------------------");
        BL_LOG_DEBUG("");
    }
}
