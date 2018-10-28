//
// Created by Sigve Røkenes on 26/10/2018.
//

#include <iostream>
#include <algorithm>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "piece.cpp"
#include "common.cpp"
#include "network/client.h"

using namespace sf;
using namespace std;

class Game {

private:

    Client * client;

    RenderTexture renderer;

    Font font;
    Text text;
    Texture texture;

    int world[10][20];
    Sprite tiles[10][20];

    Piece currentPiece;

    int score = 0;
    int streak = 0;

    int level = 0;
    int pieces = 0;

    float speed = 1;

    bool gameOver = false;

public:
    Game(Client * client){
        this->client = client;

        renderer.create(10*TILE_SIZE, 20*TILE_SIZE);

        texture.loadFromFile("assets/tile.png");
        font.loadFromFile("assets/font.otf");
        text.setFont(font);
        text.setCharacterSize(36);

        currentPiece = Piece();
    }

    void update(float delta){

        if(gameOver){
            if(Keyboard::isKeyPressed(Keyboard::Key::Enter)){
                game_reset();
            }
            return;
        }

        currentPiece.update(delta, speed, world, tiles);

        if(currentPiece.isPiecePlaced()){
            pieces++;

            if(currentPiece.isGameOver()){
                gameOver = true;
                return;
            }

            currentPiece = Piece();
            clear_lines();

            if(pieces > level*50){
                level ++;
                speed *= 1.2;
            }

            if(client != NULL)
                client->updateState(world);
        }
    }

    void game_reset(){
        gameOver = false;
        currentPiece = Piece();

        for(int x=0;x<10;x++){
            for(int y=0;y<20;y++){
                world[x][y] = 0;
            }
        }

        score = 0;
        streak = 0;
        speed = 1;
        level = 0;
        pieces = 0;
    }

    void clear_lines(){
        int total = 0;
        bool line = true;
        for(int y=0;y<20;y++){
            line = true;
            for(int x=0;x<10;x++){
                if(world[x][y] == 0){
                    line = false;
                    break;
                }
            }
            if(line){
                move_down_from(y);
                total++;
            }
        }
        if(total>0) {
            add_score(total);
            streak++;
        }else streak = 0;
    }

    void move_down_from(int y){
        for(; y > 0; y--){
            for(int x=0;x<10;x++){
                if(y==0) {
                    world[x][y] = 0;
                }else {
                    world[x][y] = world[x][y - 1];
                    tiles[x][y] = tiles[x][y - 1];
                }
            }
        }
    }

    void add_score(int lines){
        float mult = (streak == 0) ? 1 : streak * 1.25;
        if(lines == 4)
            score += (int)(500.0 * mult);
        else
            score += (int)(lines*100.0 * mult);
    }

    void draw(){
        renderer.clear(Color::Black);

        currentPiece.draw(&renderer, texture);

        for(int x=0;x<10;x++){
            for(int y=0;y<20;y++){
                if(world[x][y] > 0){
                    tiles[x][y].setTexture(texture);
                    tiles[x][y].setPosition(Vector2f(x*TILE_SIZE, y*TILE_SIZE));
                    renderer.draw(tiles[x][y]);
                }
            }
        }

        RectangleShape rect = RectangleShape(Vector2f(TILE_SIZE*10, 50));
        rect.setFillColor(Color(50,50,50));
        rect.setPosition(0, 0);
        renderer.draw(rect);

        char scr[10];
        sprintf(scr, "%06d", score);

        text.setPosition(325, 0);
        text.setString(scr);
        renderer.draw(text);

        text.setPosition(10, 0);
        sprintf(scr, "%04d", level);
        text.setString("LVL "+to_string(level));
        renderer.draw(text);

        if(gameOver){
            RectangleShape rect = RectangleShape(Vector2f(TILE_SIZE*10, TILE_SIZE*3));
            rect.setFillColor(Color::Red);
            rect.setPosition(0, 660);

            renderer.draw(rect);

            text.setCharacterSize(96);
            text.setPosition(80,700);
            text.setString("Game Over");
            renderer.draw(text);

            text.setCharacterSize(36);
            text.setPosition(80,800);
            text.setString("Press ENTER to play again");
            renderer.draw(text);
        }

    }

    RenderTexture * getRenderTexture(){
        renderer.display();
        return &renderer;
    }
};