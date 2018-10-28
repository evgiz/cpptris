//
// Created by Sigve Røkenes on 26/10/2018.
//

#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <ctype.h>

#include "network/server.h"
#include "network/client.h"
#include "game.cpp"

using namespace sf;
using namespace std;

class Lobby {

private:

    enum State {
        None,
        Join,
        Host,
        Singleplayer
    };

    enum Typing {
        Name,
        Address,
        Nothing
    };

    State state;
    Font lobbyFont;
    Text lobbyText;
    Texture texture;

    Server* server;
    Client* client;

    Game *game;
    bool gameStarted = false;

    Typing typing = Nothing;
    string typeText = "";
    string address = "127.0.0.1";

    string options[4] =
            {
            "Singleplayer",
            "Join game",
            "Host game",
            "Quit"
            };

    int selected = 0;

    bool downReleased = false;
    bool upReleased = false;
    bool backspaceReleased = false;
    bool enterReleased = false;

public:

    static bool KEYBOARD_FOCUS;

    Lobby(){
        state = None;
        lobbyFont.loadFromFile("assets/font.otf");
        lobbyText.setFont(lobbyFont);
        lobbyText.setCharacterSize(48);
        texture.loadFromFile("assets/tile.png");
    }

    void update(float delta){

        if(gameStarted){
            game->update(delta);
            return;
        }

        if(state == None) {
            if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
                if (downReleased) {
                    downReleased = false;
                    selected++;
                    if (selected > 3)selected = 3;
                }
            } else downReleased = true;

            if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                if (upReleased) {
                    upReleased = false;
                    selected--;
                    if (selected < 0)selected = 0;
                }
            } else upReleased = true;

            if (Keyboard::isKeyPressed(Keyboard::Key::Enter) || Keyboard::isKeyPressed(Keyboard::Key::Space)) {
                switch (selected) {
                    case 0:
                        state = Singleplayer;
                        game = new Game(NULL);
                        gameStarted = true;
                        break;
                    case 1:
                        state = Join;
                        typing = Address;
                        break;
                    case 2:
                        server = new Server();
                        state = Host;
                        typing = Name;
                        break;
                }
            }
            return;
        }

        if(typing != Nothing){
            if(Keyboard::isKeyPressed(Keyboard::Key::BackSpace)){
                if(backspaceReleased){
                    backspaceReleased = false;
                    typeText = typeText.substr(0, typeText.length()-1);
                }
            }else backspaceReleased = true;

            if(Keyboard::isKeyPressed(Keyboard::Key::Enter)){
                if(enterReleased) {
                    enterReleased = false;
                    if (typing == Address) {
                        typing = Name;
                        address = typeText;
                        typeText = "";
                    } else typing = Nothing;

                    if (typing == Nothing)
                        client = new Client(typeText, address);
                }
            }else enterReleased = true;
        }


        if(state == Host && typing == Nothing){
            if(Keyboard::isKeyPressed(Keyboard::Key::P)){
                server->startGame();
                game = new Game(client);
                gameStarted = true;
            }
        }

        if(state == Join && typing == Nothing ){
            if(client->isGameStarted()) {
                game = new Game(client);
                gameStarted = true;
            }else if(!client->isConnected()){
                if(Keyboard::isKeyPressed(Keyboard::Key::Escape)){
                    state = None;
                    typeText = "";
                }
            }
        }

    }

    void text_input(char c){
        if(!isalpha(c) && !isdigit(c) && !(c==' ') && !(c=='.'))
            return;
        if(typing != Nothing){
            typeText += c;
        }
    }

    void action(){
        cout << "Action: " << options[selected] <<endl;
    }

    void draw(RenderWindow &window){

        if(gameStarted){
            game->draw();

            RectangleShape outline = RectangleShape(Vector2f(TILE_SIZE*10 + 50, TILE_SIZE*20 + 50));
            outline.setFillColor(Color(50,50,50));
            outline.setPosition(25, 25);
            window.draw(outline);

            Sprite spr = Sprite(game->getRenderTexture()->getTexture());
            spr.setPosition(50,50);
            window.draw(spr);

            if(state != Singleplayer)
                drawUserWorlds(window);

            return;
        }

        lobbyText.setColor(Color(255,255,255));
        lobbyText.setCharacterSize(64);
        lobbyText.setString("C++tris");
        lobbyText.setPosition(70,550);
        window.draw(lobbyText);

        if(state == None) {
            lobbyText.setCharacterSize(48);
            for (int i = 0; i < 4; i++) {
                lobbyText.setString(options[i]);
                lobbyText.setPosition(90 + (i == selected ? 45 : 0), 700 + 60 * i);
                window.draw(lobbyText);
            }
            return;
        }

        if(typing != Nothing){
            lobbyText.setCharacterSize(48);
            lobbyText.setPosition(100, 700);
            lobbyText.setColor(Color(100,100,100));
            if(typing == Name)
                lobbyText.setString("Enter name: ");
            else
                lobbyText.setString("Enter address: ");
            window.draw(lobbyText);
            lobbyText.setColor(Color(255,255,0));
            lobbyText.setPosition(100, 750);
            lobbyText.setString(typeText+"");
            window.draw(lobbyText);
            lobbyText.setColor(Color(255,255,255));
        }

        if(state == Host){
            lobbyText.setCharacterSize(48);
            lobbyText.setPosition(70, 620);
            lobbyText.setColor(Color(255,0,0));
            lobbyText.setString("Hosting server.");
            window.draw(lobbyText);

            lobbyText.setColor(Color(255,255,255));
            lobbyText.setCharacterSize(48);
            lobbyText.setPosition(70, 850);
            lobbyText.setString("Players:");
            window.draw(lobbyText);

            lobbyText.setColor(Color(100,100,100));

            for(int i=0;i<4;i++){
                lobbyText.setPosition(70, 900+50*i);
                lobbyText.setString(server->getName(i));
                window.draw(lobbyText);
            }

            if(typing == Nothing) {
                lobbyText.setColor(Color(0, 255, 0));
                lobbyText.setCharacterSize(48);
                lobbyText.setPosition(70, 1250);
                lobbyText.setString("P to play");
                window.draw(lobbyText);
            }
        }

        if(state == Join && typing == Nothing){

            lobbyText.setCharacterSize(48);
            lobbyText.setPosition(70, 620);
            lobbyText.setColor(Color(255,0,0));

            if(client->isConnected()) {
                lobbyText.setString("Waiting for host...");
                window.draw(lobbyText);

                lobbyText.setColor(Color(255, 255, 255));
                lobbyText.setCharacterSize(48);
                lobbyText.setPosition(70, 850);
                lobbyText.setString("Players:");
                window.draw(lobbyText);

                lobbyText.setColor(Color(100, 100, 100));
                for (int i = 0; i < 4; i++) {
                    lobbyText.setPosition(70, 900 + 50 * i);
                    lobbyText.setString(client->getName(i));
                    window.draw(lobbyText);
                }
            } else {
                lobbyText.setString("Failed to connect");
                window.draw(lobbyText);

                lobbyText.setPosition(70, 820);
                lobbyText.setColor(Color(255,255,0));

                lobbyText.setString("Press escape to exit");
                window.draw(lobbyText);
            }



        }
    }


    void drawUserWorlds(RenderWindow& window){
        int tile = 24;
        int border = 10;
        int starty = 52;
        int startx = TILE_SIZE*10 + 100;

        int usr = -1;

        lobbyText.setColor(Color(255,255,255));
        lobbyText.setCharacterSize(24);

        for(int i=0;i<3;i++){
            int posx = startx + border;
            int posy = starty + border + i*tile*20 + i*75;

            usr++;
            if(client->getId() == usr) {
                usr++;
            }

            RectangleShape outline(Vector2f(tile*10 + border*2, tile*20 + border*2));
            outline.setFillColor(Color(50,50,50));
            outline.setPosition(posx-border, posy-border);
            window.draw(outline);

            Sprite block(texture);
            block.setScale(tile / 16.0, tile/16.0);

            int *world = client->getUserWorld(usr);

            for(int x=0;x<10;x++){
                for(int y=0;y<20;y++){
                    block.setPosition(posx + x*tile, posy + y*tile);

                    if(world[x + y*10]){
                        block.setColor(PIECE_COLOR[world[x+y*10]-1]);
                    }else block.setColor(Color::Black);

                    window.draw(block);

                }
            }

            lobbyText.setString(client->getName(usr));
            lobbyText.setPosition(TILE_SIZE*10 + 100, posy-45);
            window.draw(lobbyText);

        }
    }

};