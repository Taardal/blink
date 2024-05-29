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
            yaw += rotationSpeed;
        }
        if (config.keyboard->isPressed(Key::Right)) {
            yaw -= rotationSpeed;
        }
        if (config.keyboard->isPressed(Key::Q)) {
            roll += rotationSpeed;
        }
        if (config.keyboard->isPressed(Key::E)) {
            roll -= rotationSpeed;
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
        glm::quat yawRotation = glm::angleAxis(glm::radians(yaw), POSITIVE_Y_AXIS);
        glm::quat pitchRotation = glm::angleAxis(glm::radians(pitch), POSITIVE_X_AXIS);
        glm::quat rollRotation = glm::angleAxis(glm::radians(roll), POSITIVE_Z_AXIS);

        orientation = yawRotation * pitchRotation * rollRotation;

        forwardDirection = orientation * NEGATIVE_Z_AXIS;
        rightDirection = orientation * POSITIVE_X_AXIS;
        upDirection = orientation * POSITIVE_Y_AXIS;
    }

    void SceneCamera::calculateView() {
        view = glm::lookAt(position, position + forwardDirection, upDirection);
    }

    void SceneCamera::calculateProjection() {
        projection = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearClip, farClip);
    }

    void SceneCamera::calculateAspectRatio() {
        WindowSize size = config.window->getSizeInPixels();
        width = (float) size.width;
        height = (float) size.height;
        aspectRatio = width / height;
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
        BL_LOG_DEBUG("Move speed: {}", moveSpeed);
        BL_LOG_DEBUG("Rotation speed: {}", rotationSpeed);
        BL_LOG_DEBUG("");
        BL_LOG_DEBUG("Field of view: {}°", fieldOfView);
        BL_LOG_DEBUG("Near clip: {}", nearClip);
        BL_LOG_DEBUG("Far clip: {}", farClip);
        BL_LOG_DEBUG("");
        BL_LOG_DEBUG("Width: {}", width);
        BL_LOG_DEBUG("Height: {}", height);
        BL_LOG_DEBUG("Aspect ratio: {}", aspectRatio);
        BL_LOG_DEBUG("");
        BL_LOG_DEBUG("--------------------------------------------------------------------------------------------------------------");
        BL_LOG_DEBUG("");
    }
}
