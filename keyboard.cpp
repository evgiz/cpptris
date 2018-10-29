
#include "keyboard.h"
#include <SFML/Window.hpp>

using namespace sf;


KeyboardManager::State keyState[Keyboard::KeyCount];

void KeyboardManager::keyboardUpdate(bool focus){
    for(int i=0; i < Keyboard::KeyCount; i++){
        if(!focus){
            keyState[i] = KeyboardManager::None;
            continue;
        }

        if(Keyboard::isKeyPressed((Keyboard::Key)i)){
            if(keyState[i] == KeyboardManager::None)
                keyState[i] = KeyboardManager::Down;
            else
                keyState[i] = KeyboardManager::Pressed;
        }else
            keyState[i] = KeyboardManager::None;
    }
}

bool  KeyboardManager::keyDown(Keyboard::Key key){
   if(keyState[key] == KeyboardManager::Down){
       keyState[key] = KeyboardManager::Pressed;
       return true;
   }
   return false;
}

bool  KeyboardManager::keyPressed(Keyboard::Key key){
    return keyState[key] != KeyboardManager::None;
}
