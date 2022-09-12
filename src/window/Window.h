#pragma once

#include "Config.h"
#include "GLFW/glfw3.h"
#include <string>

namespace Blink
{
    class Window
    {
    private:
        GLFWwindow* glfwWindow;

    public:
        Window();

        bool initialize(const Config& config);

        void terminate() const;

        void onUpdate() const;

        bool shouldClose() const;

    private:
        static void onGlfwError(int32_t error, const char* description);

        static void onKeyChange(GLFWwindow* glfwWindow, int32_t key, int32_t scanCode, int32_t action, int32_t mods);
    };
}