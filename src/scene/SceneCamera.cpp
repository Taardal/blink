#include "pch.h"
#include "window/Mouse.h"
#include "scene/SceneCamera.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Blink {
    SceneCamera::SceneCamera(const SceneCameraConfig& config) : config(config) {
        mouseSensitivity = 0.1f;
        updateAspectRatio();
        updateDirections();
    }

    glm::mat4 SceneCamera::getView() const {
        return view;
    }

    glm::mat4 SceneCamera::getProjection() const {
        return projection;
    }

    glm::mat4 SceneCamera::getRotation() const {
        glm::mat4 rotationMatrix(1.0f);
        rotationMatrix[0] = glm::vec4(rightDirection, 0.0f);
        rotationMatrix[1] = glm::vec4(upDirection, 0.0f);
        rotationMatrix[2] = glm::vec4(-forwardDirection, 0.0f); // Negate forward to align with Vulkan conventions
        return glm::transpose(rotationMatrix);
    }

    void SceneCamera::setPosition(const glm::vec3& position) {
        this->position = position;
    }

    void SceneCamera::setForwardDirection(const glm::vec3& forwardDirection) {
        this->forwardDirection = forwardDirection;
    }

    void SceneCamera::onEvent(Event& event) {
        if (event.type == EventType::WindowResize) {
            updateAspectRatio();
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

        if (inputEnabled) {
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

        forwardDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        forwardDirection.y = sin(glm::radians(pitch));
        forwardDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        forwardDirection = glm::normalize(forwardDirection);

        rightDirection = glm::normalize(glm::cross(forwardDirection, worldUpDirection));
        upDirection = glm::normalize(glm::cross(rightDirection, forwardDirection));

        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), forwardDirection);
        upDirection = glm::vec3(rollMatrix * glm::vec4(upDirection, 0.0f));
        rightDirection = glm::vec3(rollMatrix * glm::vec4(rightDirection, 0.0f));

        view = glm::lookAt(position, position + forwardDirection, upDirection);
        projection = glm::perspective(fieldOfView, aspectRatio, nearClip, farClip);

        logState();
    }

    void SceneCamera::processKeyboardInput(float timestep) {
    }

    void SceneCamera::updateDirections() {
    }

    void SceneCamera::updateAspectRatio() {
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
}
