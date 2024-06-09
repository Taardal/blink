#pragma once

#include "window/Window.h"

#include <glm/glm.hpp>

namespace Blink {
    // enum CursorMode {
    //     None = 0,
    //     Normal = 1,
    //     Hidden = 2,
    // };

    enum MouseButton {
        None = 0,
        Button_1 = GLFW_MOUSE_BUTTON_1,
        Button_2 = GLFW_MOUSE_BUTTON_2,
        Button_3 = GLFW_MOUSE_BUTTON_3,
        Button_4 = GLFW_MOUSE_BUTTON_4,
        Button_5 = GLFW_MOUSE_BUTTON_5,
        Button_6 = GLFW_MOUSE_BUTTON_6,
        Button_7 = GLFW_MOUSE_BUTTON_7,
        Button_8 = GLFW_MOUSE_BUTTON_8,
        Last = Button_8,
        Left = Button_1,
        Right = Button_2,
        Middle = Button_3,
    };

    struct MouseConfig {
        Window* window = nullptr;
    };

    class Mouse {
    private:
        MouseConfig config;
        std::vector<MouseButton> buttons;
        std::unordered_map<std::string, MouseButton> buttonsByName;
        std::unordered_map<MouseButton, std::string> namesByButton;

    public:
        explicit Mouse(const MouseConfig& config);

        std::vector<MouseButton> getButtons() const;

        std::unordered_map<std::string, MouseButton> getButtonsByName() const;

        std::unordered_map<MouseButton, std::string> getBamesByButton() const;

        glm::vec2 getPosition() const;

        void setCursorHidden(bool hidden) const;

        bool isPressed(MouseButton mouseButton) const;

        bool isPressed(uint16_t mouseButton) const;

        bool isPressed(const std::string& mouseButtonName) const;

        std::string getName(MouseButton mouseButton) const;

        std::string getName(uint16_t mouseButton) const;

        static std::string getButtonName(MouseButton mouseButton);

        static std::string getButtonName(uint16_t mouseButton);

    private:
        static std::vector<MouseButton> createButtons();

        static std::unordered_map<std::string, MouseButton> createButtonsByName();

        static std::unordered_map<MouseButton, std::string> createNamesByButton();
    };
}
