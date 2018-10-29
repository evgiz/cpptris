
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <SFML/Window.hpp>

namespace KeyboardManager {
    enum State {
        None,
        Down,
        Pressed
    };
    void keyboardUpdate(bool focus);
    bool keyDown(sf::Keyboard::Key key);
    bool keyPressed(sf::Keyboard::Key key);
};

#endif //KEYBOARD_H
