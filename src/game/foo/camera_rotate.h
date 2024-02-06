// ... (existing code)

class Camera {
public:
    // ... (existing members)

    void processInput() {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processKeyboardInput();
        // Consider adding a function for processing mouse input here
        updateCameraVectors();
    }

private:
    // ... (existing members)

    float yaw;    // Yaw angle (horizontal rotation)

    // ... (existing code)

    void processKeyboardInput() {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPosition += cameraSpeed * deltaTime * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPosition -= cameraSpeed * deltaTime * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            yaw += glm::radians(5.0f);  // Rotate to the right around Y-axis
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            yaw -= glm::radians(5.0f);  // Rotate to the left around Y-axis
        }
    }

    // ... (existing code)

    void updateCameraVectors() {
        // Update the camera's front, right, and up vectors based on yaw and pitch
        glm::vec3 front;
        front.x = cos(yaw) * cos(pitch);
        front.y = sin(pitch);
        front.z = sin(yaw) * cos(pitch);
        cameraFront = glm::normalize(front);
        cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
    }

    // ... (existing code)
};
