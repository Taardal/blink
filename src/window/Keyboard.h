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
        R = GLFW_KEY_R,
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
        std::vector<Key> keys = {
            Key::Space,
            Key::Up,
            Key::Down,
            Key::Left,
            Key::Right,
            Key::Q,
            Key::W,
            Key::E,
            Key::R,
            Key::A,
            Key::S,
            Key::D,
            Key::H,
            Key::J,
            Key::K,
            Key::L,
            Key::C,
            Key::Num_1,
            Key::Num_2,
            Key::Num_3,
            Key::Num_4,
            Key::Num_5,
            Key::Num_6,
            Key::Num_7,
            Key::Num_8,
            Key::Num_9,
            Key::Numpad_1,
            Key::Numpad_2,
            Key::Numpad_3,
            Key::Numpad_4,
            Key::Numpad_5,
            Key::Numpad_6,
            Key::Numpad_7,
            Key::Numpad_8,
            Key::Numpad_9,
        };
        std::unordered_map<std::string, Key> keysByName = {
            { "Space", Key::Space },
            { "Up", Key::Up },
            { "Down", Key::Down },
            { "Left", Key::Left },
            { "Right", Key::Right },
            { "Q", Key::Q },
            { "W", Key::W },
            { "E", Key::E },
            { "R", Key::R },
            { "A", Key::A },
            { "S", Key::S },
            { "D", Key::D },
            { "H", Key::H },
            { "J", Key::J },
            { "K", Key::K },
            { "L", Key::L },
            { "C", Key::C },
            { "Num_1", Key::Num_1 },
            { "Num_2", Key::Num_2 },
            { "Num_3", Key::Num_3 },
            { "Num_4", Key::Num_4 },
            { "Num_5", Key::Num_5 },
            { "Num_6", Key::Num_6 },
            { "Num_7", Key::Num_7 },
            { "Num_8", Key::Num_8 },
            { "Num_9", Key::Num_9 },
            { "Numpad_1", Key::Numpad_1 },
            { "Numpad_2", Key::Numpad_2 },
            { "Numpad_3", Key::Numpad_3 },
            { "Numpad_4", Key::Numpad_4 },
            { "Numpad_5", Key::Numpad_5 },
            { "Numpad_6", Key::Numpad_6 },
            { "Numpad_7", Key::Numpad_7 },
            { "Numpad_8", Key::Numpad_8 },
            { "Numpad_9", Key::Numpad_9 },
        };

    private:
        Window* window;

    public:
        Keyboard(Window* window);

        bool initialize();

        void terminate() const;

        bool isPressed(Key key) const;

        bool isPressed(uint16_t key) const;

        bool isPressed(const std::string& keyName) const;

        const std::vector<Key>& getKeys() const {
            return keys;
        }

        const std::unordered_map<std::string, Key>& getKeysByName() const {
            return keysByName;
        }
    };
}
