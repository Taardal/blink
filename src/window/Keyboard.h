#pragma once

#include "Window.h"

namespace Blink {
    enum class Key {
        None = 0,
        Space = GLFW_KEY_SPACE,
        Up = GLFW_KEY_UP,
        Down = GLFW_KEY_DOWN,
        Left = GLFW_KEY_LEFT,
        Right = GLFW_KEY_RIGHT,
        Q = GLFW_KEY_Q,
        W = GLFW_KEY_W,
        E = GLFW_KEY_E,
        A = GLFW_KEY_A,
        S = GLFW_KEY_S,
        D = GLFW_KEY_D,
        H = GLFW_KEY_H,
        J = GLFW_KEY_J,
        K = GLFW_KEY_K,
        L = GLFW_KEY_L,
        C = GLFW_KEY_C,
    };

    class Keyboard {
    private:
        Window* window;

    public:
        Keyboard(Window* window);

        bool initialize() const;

        void terminate() const;

        bool isPressed(Key key) const;
    };
}
