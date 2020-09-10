#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>

namespace blink
{
    class GraphicsContext
    {
    public:
        struct Config
        {
            std::string GlslVersion;
            int VersionMajor = 0;
            int VersionMinor = 0;
        };

    private:
        const Config& config;

    public:
        explicit GraphicsContext(const Config& config);

        void Init(GLFWwindow* glfwWindow) const;

        [[nodiscard]] const Config& GetConfig() const;

    private:
        void InitGlad() const;

        void LogContext() const;
    };
}


