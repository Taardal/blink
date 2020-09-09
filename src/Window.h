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
        const Config& config;
        GraphicsContext* graphicsContext;
        GLFWwindow* glfwWindow;

    public:
        Window(const Config& config, GraphicsContext* graphicsContext);

        virtual ~Window();

        void OnUpdate() const;

    private:
        void InitGlfw() const;

        static void OnGlfwError(int32_t error, const char* description);

        void SetGlfwWindowHints(GraphicsContext* graphicsContext) const;

        [[nodiscard]] GLFWwindow* CreateGlfwWindow() const;

        void DestroyGlfwWindow() const;

        void TerminateGlfw() const;
    };
}