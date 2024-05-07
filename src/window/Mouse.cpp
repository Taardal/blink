#include "Mouse.h"

#include <GLFW/glfw3.h>

namespace Blink {
    Mouse::Mouse(const MouseConfig& config): config(config) {
    }

    glm::vec2 Mouse::getPosition() const {
        return config.window->getMousePosition();
    }

    void Mouse::setCursorHidden(bool hidden) const {
        config.window->setMouseCursorHidden(hidden);
    }

    bool Mouse::isPressed(MouseButton mouseButton) const {
        return config.window->isMouseButtonPressed((uint16_t) mouseButton);
    }

    bool Mouse::isPressed(uint16_t mouseButton) const {
        return config.window->isMouseButtonPressed(mouseButton);
    }

    bool Mouse::isPressed(const std::string& mouseButtonName) const {
        MouseButton mouseButton = buttonsByName.at(mouseButtonName);
        return config.window->isMouseButtonPressed((uint16_t) mouseButton);
    }

    std::string Mouse::getButtonName(MouseButton mouseButton) const {
        return namesByButton.at(mouseButton);
    }

    std::string Mouse::getButtonName(uint16_t mouseButton) const {
        return getButtonName((MouseButton) mouseButton);
    }

    std::string Mouse::getMouseButtonName(uint16_t mouseButton) {
        return getMouseButtonName((MouseButton) mouseButton);
    }

    std::string Mouse::getMouseButtonName(MouseButton mouseButton) {
        switch (mouseButton) {
            case MouseButton::Button_1:
                return "Button_1";
            case MouseButton::Button_2:
                return "Button_2";
            case MouseButton::Button_3:
                return "Button_3";
            case MouseButton::Button_4:
                return "Button_4";
            case MouseButton::Button_5:
                return "Button_5";
            case MouseButton::Button_6:
                return "Button_6";
            case MouseButton::Button_7:
                return "Button_7";
            case MouseButton::Button_8:
                return "Button_8";
            // case MouseButton::Last:
            //     return "Last";
            // case MouseButton::Left:
            //     return "Left";
            // case MouseButton::Right:
            //     return "Right";
            // case MouseButton::Middle:
            //     return "Middle";
            default:
                return "";
        }
    }

    std::vector<MouseButton> Mouse::createButtons() {
        return {
            MouseButton::Button_1,
            MouseButton::Button_2,
            MouseButton::Button_3,
            MouseButton::Button_4,
            MouseButton::Button_5,
            MouseButton::Button_6,
            MouseButton::Button_7,
            MouseButton::Button_8,
            MouseButton::Last,
            MouseButton::Left,
            MouseButton::Right,
            MouseButton::Middle,
        };
    }

    std::unordered_map<std::string, MouseButton> Mouse::createButtonsByName() {
        return {
            {"Button_1", MouseButton::Button_1},
            {"Button_2", MouseButton::Button_2},
            {"Button_3", MouseButton::Button_3},
            {"Button_4", MouseButton::Button_4},
            {"Button_5", MouseButton::Button_5},
            {"Button_6", MouseButton::Button_6},
            {"Button_7", MouseButton::Button_7},
            {"Button_8", MouseButton::Button_8},
            {"Last", MouseButton::Last},
            {"Left", MouseButton::Left},
            {"Right", MouseButton::Right},
            {"Middle", MouseButton::Middle},
        };
    }

    std::unordered_map<MouseButton, std::string> Mouse::createNamesByButton() {
        return {
            {MouseButton::Button_1, "Button_1"},
            {MouseButton::Button_2, "Button_2"},
            {MouseButton::Button_3, "Button_3"},
            {MouseButton::Button_4, "Button_4"},
            {MouseButton::Button_5, "Button_5"},
            {MouseButton::Button_6, "Button_6"},
            {MouseButton::Button_7, "Button_7"},
            {MouseButton::Button_8, "Button_8"},
            {MouseButton::Last, "Last"},
            {MouseButton::Left, "Left"},
            {MouseButton::Right, "Right"},
            {MouseButton::Middle, "Middle"},
        };
    }
}
