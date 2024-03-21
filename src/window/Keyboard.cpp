#include "Keyboard.h"

namespace Blink {
    Keyboard::Keyboard(
        Window* window
    ) : window(window),
        keys(createKeys()),
        keysByName(createKeysByName()),
        namesByKey(createNamesByKey()) {
    }

    bool Keyboard::isPressed(Key key) const {
        return window->isKeyPressed((uint16_t) key);
    }

    bool Keyboard::isPressed(uint16_t key) const {
        return window->isKeyPressed(key);
    }

    bool Keyboard::isPressed(const std::string& keyName) const {
        Key key = keysByName.at(keyName);
        return window->isKeyPressed((uint16_t) key);
    }

    std::string Keyboard::getName(Key key) const {
        return namesByKey.at(key);
    }

    std::string Keyboard::getName(uint16_t key) const {
        return getName((Key) key);
    }

    std::string Keyboard::getKeyName(uint16_t key) {
        return getKeyName((Key) key);
    }

    std::string Keyboard::getKeyName(Key key) {
        switch (key) {
            case Key::Space:
                return "Space";
            case Key::Escape:
                return "Escape";
            case Key::Enter:
                return "Enter";
            case Key::Tab:
                return "Tab";
            case Key::Backspace:
                return "Backspace";
            case Key::Up:
                return "Up";
            case Key::Down:
                return "Down";
            case Key::Left:
                return "Left";
            case Key::Right:
                return "Right";
            case Key::Q:
                return "Q";
            case Key::W:
                return "W";
            case Key::E:
                return "E";
            case Key::R:
                return "R";
            case Key::A:
                return "A";
            case Key::S:
                return "S";
            case Key::D:
                return "D";
            case Key::H:
                return "H";
            case Key::J:
                return "J";
            case Key::K:
                return "K";
            case Key::L:
                return "L";
            case Key::C:
                return "C";
            case Key::Num_1:
                return "Num_1";
            case Key::Num_2:
                return "Num_2";
            case Key::Num_3:
                return "Num_3";
            case Key::Num_4:
                return "Num_4";
            case Key::Num_5:
                return "Num_5";
            case Key::Num_6:
                return "Num_6";
            case Key::Num_7:
                return "Num_7";
            case Key::Num_8:
                return "Num_8";
            case Key::Num_9:
                return "Num_9";
            case Key::Numpad_1:
                return "Numpad_1";
            case Key::Numpad_2:
                return "Numpad_2";
            case Key::Numpad_3:
                return "Numpad_3";
            case Key::Numpad_4:
                return "Numpad_4";
            case Key::Numpad_5:
                return "Numpad_5";
            case Key::Numpad_6:
                return "Numpad_6";
            case Key::Numpad_7:
                return "Numpad_7";
            case Key::Numpad_8:
                return "Numpad_8";
            case Key::Numpad_9:
                return "Numpad_9";
            default:
                return "";
        }
    }

    std::vector<Key> Keyboard::createKeys() {
        return {
            Key::Space,
            Key::Escape,
            Key::Enter,
            Key::Tab,
            Key::Backspace,
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
    }

    std::unordered_map<std::string, Key> Keyboard::createKeysByName() {
        return {
            { "Space", Key::Space },
            { "Escape", Key::Escape },
            { "Enter", Key::Enter },
            { "Tab", Key::Tab },
            { "Backspace", Key::Backspace },
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
    }

    std::unordered_map<Key, std::string> Keyboard::createNamesByKey() {
        return {
            { Key::Space, "Space" },
            { Key::Escape, "Escape" },
            { Key::Enter, "Enter" },
            { Key::Tab, "Tab" },
            { Key::Backspace, "Backspace" },
            { Key::Up, "Up" },
            { Key::Down, "Down" },
            { Key::Left, "Left" },
            { Key::Right, "Right" },
            { Key::Q, "Q" },
            { Key::W, "W" },
            { Key::E, "E" },
            { Key::R, "R" },
            { Key::A, "A" },
            { Key::S, "S" },
            { Key::D, "D" },
            { Key::H, "H" },
            { Key::J, "J" },
            { Key::K, "K" },
            { Key::L, "L" },
            { Key::C, "C" },
            { Key::Num_1, "Num_1" },
            { Key::Num_2, "Num_2" },
            { Key::Num_3, "Num_3" },
            { Key::Num_4, "Num_4" },
            { Key::Num_5, "Num_5" },
            { Key::Num_6, "Num_6" },
            { Key::Num_7, "Num_7" },
            { Key::Num_8, "Num_8" },
            { Key::Num_9, "Num_9" },
            { Key::Numpad_1, "Numpad_1" },
            { Key::Numpad_2, "Numpad_2" },
            { Key::Numpad_3, "Numpad_3" },
            { Key::Numpad_4, "Numpad_4" },
            { Key::Numpad_5, "Numpad_5" },
            { Key::Numpad_6, "Numpad_6" },
            { Key::Numpad_7, "Numpad_7" },
            { Key::Numpad_8, "Numpad_8" },
            { Key::Numpad_9, "Numpad_9" },
        };
    }
}
