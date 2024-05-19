#include "pch.h"
#include "window/Mouse.h"
#include "scene/SceneCamera.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Blink {
    SceneCamera::SceneCamera(const SceneCameraConfig& config) : config(config) {
        mouseSensitivity = 0.1f;
        calculateAspectRatio();
    }

    glm::mat4 SceneCamera::getView() const {
        return view;
    }

    glm::mat4 SceneCamera::getProjection() const {
        return projection;
    }

    void SceneCamera::onEvent(Event& event) {
        if (event.type == EventType::WindowResize) {
            calculateAspectRatio();
        }
    }

    void SceneCamera::update(double timestep) {

        static bool firstMouse = true;
        if (firstMouse) {
            lastMousePosition = config.mouse->getPosition();
            firstMouse = false;
        }
        if (config.mouse->isPressed(MouseButton::Right)) {
            mousePosition = config.mouse->getPosition();
            glm::vec2 delta = (mousePosition - lastMousePosition) * mouseSensitivity;
            lastMousePosition = mousePosition;

            yaw += delta.x;
            pitch += -delta.y;
        }

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

        forwardDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        forwardDirection.y = sin(glm::radians(pitch));
        forwardDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        forwardDirection = glm::normalize(forwardDirection);

        rightDirection = glm::normalize(glm::cross(forwardDirection, worldUpDirection));
        upDirection = glm::normalize(glm::cross(rightDirection, forwardDirection));

        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), forwardDirection);
        rightDirection = glm::vec3(rollMatrix * glm::vec4(rightDirection, 0.0f));
        upDirection = glm::vec3(rollMatrix * glm::vec4(upDirection, 0.0f));

        view = glm::lookAt(position, position + forwardDirection, upDirection);
        projection = glm::perspective(fieldOfView, aspectRatio, nearClip, farClip);

        if (loggingEnabled) {
            logState();
        }
    }

    void SceneCamera::calculateAspectRatio() {
        WindowSize windowSize = config.window->getSizeInPixels();
        aspectRatio = (float) windowSize.width / (float) windowSize.height;
    }

    void SceneCamera::logState() const {
        BL_LOG_DEBUG("===============[ C A M E R A ]===============");
        BL_LOG_DEBUG("--");
        BL_LOG_DEBUG("-- Position: [{}, {}, {}]", position.x, position.y, position.z);
        BL_LOG_DEBUG("--");
        BL_LOG_DEBUG("-- Forward direction: [{}, {}, {}]", forwardDirection.x, forwardDirection.y, forwardDirection.z);
        BL_LOG_DEBUG("-- Up direction: [{}, {}, {}]", upDirection.x, upDirection.y, upDirection.z);
        BL_LOG_DEBUG("-- Right direction: [{}, {}, {}]", rightDirection.x, rightDirection.y, rightDirection.z);
        BL_LOG_DEBUG("--");
        BL_LOG_DEBUG("-- Yaw: {}°", yaw);
        BL_LOG_DEBUG("-- Pitch: {}°", pitch);
        BL_LOG_DEBUG("-- Roll: {}°", roll);
        BL_LOG_DEBUG("--");
    }

    void SceneCamera::setMoveSpeed(float moveSpeed) {
        this->moveSpeed = moveSpeed;
    }

    void SceneCamera::setRotationSpeed(float rotationSpeed) {
        this->rotationSpeed = rotationSpeed;
    }

    void SceneCamera::setDirections(const Directions& directions) {
        this->forwardDirection = directions.forwardDirection;
        this->rightDirection = directions.rightDirection;
        this->upDirection = directions.upDirection;
        this->worldUpDirection = directions.worldUpDirection;

        this->directions = directions;
        calculateEulerAngles();
    }

    void SceneCamera::setEulerAngles(const EulerAngles& eulerAngles) {
        this->yaw = eulerAngles.yaw;
        this->pitch = eulerAngles.pitch;
        this->roll = eulerAngles.roll;

        this->eulerAngles = eulerAngles;
        calculateDirections();
    }

    void SceneCamera::setFrustum(const Frustum& frustum) {
        this->frustum = frustum;

        this->fieldOfView = frustum.fieldOfView;
        this->nearClip = frustum.nearClip;
        this->farClip = frustum.farClip;
    }

    void SceneCamera::setPosition(const glm::vec3& position) {
        this->position = position;
    }

    void SceneCamera::setWorldUpDirection(const glm::vec3& worldUpDirection) {
        this->worldUpDirection = worldUpDirection;
    }

    void SceneCamera::setForwardDirection(const glm::vec3& forwardDirection) {
        this->forwardDirection = forwardDirection;
        rightDirection = glm::normalize(glm::cross(forwardDirection, worldUpDirection));
        upDirection = glm::normalize(glm::cross(rightDirection, forwardDirection));
        calculateEulerAngles();
    }

    void SceneCamera::setRightDirection(const glm::vec3& rightDirection) {
        this->rightDirection = rightDirection;
        calculateEulerAngles();
    }

    void SceneCamera::setUpDirection(const glm::vec3& upDirection) {
        this->upDirection = upDirection;
        calculateEulerAngles();
    }

    void SceneCamera::calculateDirections() {
        forwardDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        forwardDirection.y = sin(glm::radians(pitch));
        forwardDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        forwardDirection = glm::normalize(forwardDirection);

        rightDirection = glm::normalize(glm::cross(forwardDirection, worldUpDirection));
        upDirection = glm::normalize(glm::cross(rightDirection, forwardDirection));

        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), forwardDirection);
        upDirection = glm::vec3(rollMatrix * glm::vec4(upDirection, 0.0f));
        rightDirection = glm::vec3(rollMatrix * glm::vec4(rightDirection, 0.0f));
    }

    void SceneCamera::calculateEulerAngles() {
        yaw = glm::degrees(std::atan2(forwardDirection.z, forwardDirection.x));
        pitch = glm::degrees(std::asin(-forwardDirection.y));
        roll = glm::degrees(std::atan2(rightDirection.y, upDirection.y));
    }
}
