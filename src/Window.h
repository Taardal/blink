#pragma once
#include "GraphicsContext.h"
#include <GLFW/glfw3.h>
#include <string>

namespace blink
{
    class Window
    {
    public:
        struct Config
        {
            std::string Title;
            int Width = 0;
            int Height = 0;
            bool Maximized = false;
        };

    private:
        Window::Config config;
        GLFWwindow* glfwWindow;

    public:
        Window();

        virtual ~Window();

        void Init(const Window::Config& windowConfig, const GraphicsContext::Config& graphicsConfig);

        void OnUpdate() const;

    private:
        void InitGlfw() const;

        static void OnGlfwError(int32_t error, const char* description);

        void SetGlfwWindowHints(const GraphicsContext::Config& graphicsConfig) const;

        [[nodiscard]] GLFWwindow* CreateGlfwWindow() const;

        void DestroyGlfwWindow() const;

        void TerminateGlfw() const;
    };
}