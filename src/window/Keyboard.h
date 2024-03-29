#pragma once

#include "Window.h"

namespace Blink {
    enum class Key {
        None = 0,
        Space = GLFW_KEY_SPACE,
        Escape = GLFW_KEY_ESCAPE,
        Enter = GLFW_KEY_ENTER,
        Tab = GLFW_KEY_TAB,
        Backspace = GLFW_KEY_BACKSPACE,
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
        Window* window;
        std::vector<Key> keys;
        std::unordered_map<std::string, Key> keysByName;
        std::unordered_map<Key, std::string> namesByKey;

    public:
        explicit Keyboard(Window* window);

        const std::vector<Key>& getKeys() const {
            return keys;
        }

        const std::unordered_map<std::string, Key>& getKeysByName() const {
            return keysByName;
        }

        std::unordered_map<Key, std::string> getNamesByKey() const {
            return namesByKey;
        }

        bool isPressed(Key key) const;

        bool isPressed(uint16_t key) const;

        bool isPressed(const std::string& keyName) const;

        std::string getName(Key key) const;

        std::string getName(uint16_t key) const;

        static std::string getKeyName(Key key);

        static std::string getKeyName(uint16_t key);

    private:
        static std::vector<Key> createKeys();

        static std::unordered_map<std::string, Key> createKeysByName();

        static std::unordered_map<Key, std::string> createNamesByKey();
    };
}
