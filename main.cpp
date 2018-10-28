#include <iostream>
#include <algorithm>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "lobby.cpp"

using namespace sf;
using namespace std;

int main() {

    RenderWindow window(VideoMode(1200, 1700), "Tetris");
    Lobby lobby;

    Clock clock;
    window.setFramerateLimit(60);
    float delta = 0;

    Color background = Color(10,10,10);

    bool focus = false;

    while(window.isOpen()){
        Event event;

        while(window.pollEvent(event)){
            if(event.type == Event::Closed) {
                window.close();
            }
            if(event.type == Event::LostFocus) {
                focus = false;
            }
            if(event.type == Event::GainedFocus) {
                focus = true;
            }
            if(event.type == Event::TextEntered){
                if(focus) {
                    if (event.text.unicode < 128)
                        lobby.text_input(static_cast<char>(event.text.unicode));
                }
            }
        }

        delta = clock.getElapsedTime().asMilliseconds() / 1000.0f;
        lobby.update(min(delta, 1.0f/30.0f));
        clock.restart();

        window.clear(background);
        lobby.draw(window);
        window.display();

    }

    return 0;
}


