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
        Num_1 = GLFW_KEY_1,
        Num_2 = GLFW_KEY_2,
        Num_3 = GLFW_KEY_3,
        Num_4 = GLFW_KEY_4,
        Num_5 = GLFW_KEY_5,
        Num_6 = GLFW_KEY_6,
        Num_7 = GLFW_KEY_7,
        Num_8 = GLFW_KEY_8,
        Num_9 = GLFW_KEY_9,
        Numpad_1 = GLFW_KEY_KP_1,
        Numpad_2 = GLFW_KEY_KP_2,
        Numpad_3 = GLFW_KEY_KP_3,
        Numpad_4 = GLFW_KEY_KP_4,
        Numpad_5 = GLFW_KEY_KP_5,
        Numpad_6 = GLFW_KEY_KP_6,
        Numpad_7 = GLFW_KEY_KP_7,
        Numpad_8 = GLFW_KEY_KP_8,
        Numpad_9 = GLFW_KEY_KP_9,
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
