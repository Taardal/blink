#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(GLFWwindow* window, const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f)
        : window(window), position(position), yaw(yaw), pitch(pitch), speed(2.5f), sensitivity(0.1f) {
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    void processInput(float deltaTime) {
        processKeyboardInput(deltaTime);
        processMouseInput();
    }

private:
    GLFWwindow* window;
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;

    void updateCameraVectors() {
        // Calculate the new front vector
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);

        // Recalculate the right and up vectors
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        up = glm::normalize(glm::cross(right, front));
    }

    void processKeyboardInput(float deltaTime) {
        float velocity = speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= front * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= right * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += right * velocity;
    }

    void processMouseInput() {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        static double lastX = xpos;
        static double lastY = ypos;

        double xoffset = xpos - lastX;
        double yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top

        lastX = xpos;
        lastY = ypos;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Clamp pitch to prevent camera flipping
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        updateCameraVectors();
    }
};
