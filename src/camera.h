#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    Camera(GLFWwindow* window, float fov, float aspectRatio, float nearClip, float farClip)
        : window(window), fov(fov), aspectRatio(aspectRatio), nearClip(nearClip), farClip(farClip),
          cameraPosition(glm::vec3(0.0f, 0.0f, 3.0f)), cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
          cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)), cameraSpeed(2.5f), deltaTime(0.0f), lastFrame(0.0f)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetScrollCallback(window, scrollCallback);
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    }

    glm::mat4 getProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
    }

    void processInput() {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processKeyboardInput();
    }

private:
    GLFWwindow* window;
    float fov;
    float aspectRatio;
    float nearClip;
    float farClip;

    glm::vec3 cameraPosition;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

    float cameraSpeed;
    float deltaTime;
    float lastFrame;

    static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
        // Handle mouse movement
        // ...
    }

    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        // Handle mouse scroll
        // ...
    }

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
    }
};
