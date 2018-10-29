
#include <iostream>
#include <algorithm>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "piece.h"
#include "common.cpp"
#include "network/client.h"
#include "keyboard.h"

using namespace sf;
using namespace std;

class Game {

private:

    Client * client;

    RenderTexture renderer;

    Font font;
    Text text;
    Texture texture;
    Texture textureBlocked;

    int world[10][20];
    Sprite tiles[10][20];

    Piece currentPiece;

    int score = 0;
    int streak = 0;

    int level = 0;
    int pieces = 0;

    float speed = 1;

    float blockSenderTimer = 0;

    float pieceClientUpdateTimer = 0;

    bool gameOver = false;
    bool useClient = false;

public:
    Game(Client * client){
        this->client = client;

        if(client != NULL)
            useClient = true;

        renderer.create(10*TILE_SIZE, 20*TILE_SIZE);

        texture.loadFromFile("assets/tile.png");
        textureBlocked.loadFromFile("assets/blocked.png");

        font.loadFromFile("assets/font.otf");
        text.setFont(font);
        text.setCharacterSize(36);

        currentPiece = Piece();

        for(int x=0;x<10;x++){
            for(int y=0;y<20;y++){
                tiles[x][y].setScale(TILE_SIZE/16.0, TILE_SIZE/16.0);
            }
        }

        cout << "Started new game, using client: " << useClient << endl;

        game_reset();
    }

    void update(float delta){

        if(gameOver){
            if(KeyboardManager::keyDown(Keyboard::Key::Enter)){
                game_reset();
                useClient = false;
            }
            blockSenderTimer = 0;
            return;
        }
        currentPiece.update(delta, speed, world);

        if(useClient){
            pieceClientUpdateTimer += delta;

            if(pieceClientUpdateTimer>1/30.0){
                pieceClientUpdateTimer = 0;
                client->updatePieceState(&currentPiece);
            }
        }

        if(blockSenderTimer>0){
            blockSenderTimer -= delta;
        }

        if(currentPiece.isPiecePlaced()){
            pieces++;

            if(currentPiece.isGameOver()){
                gameOver = true;
                if(useClient)
                    client->sendGameOver();
                return;
            }

            currentPiece = Piece();
            clear_lines();

            while(useClient && client->addBlock()){
                insertBlocked();
                blockSenderTimer = 2.0f;
            }

            if(pieces > level*50){
                level ++;
                speed *= 1.2;
            }

            if(useClient)
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
        bool line;
        for(int y=0;y<20;y++){
            line = true;
            for(int x=0;x<10;x++){
                if(world[x][y] == 0 || world[x][y] == 10){
                    line = false;
                    break;
                }
            }
            if(line){
                moveWorldDown(y);
                total++;
            }
        }
        if(total>0) {
            add_score(total);
            streak++;

            if(useClient && total==4) {
                client->sendBlock();
            }
        }else
            streak = 0;
    }

    void moveWorldDown(int y){
        for(; y > 0; y--){
            for(int x=0;x<10;x++){
                if(y==0) {
                    world[x][y] = 0;
                }else {
                    world[x][y] = world[x][y - 1];
                }
            }
        }
    }

    void insertBlocked(){
        for(int y=0; y < 20; y++){
            for(int x=0;x<10;x++){
                if(y==19) {
                    world[x][y] = 10;
                }else {
                    if(y == 0 && world[x][y]){
                        gameOver = true;
                        if(useClient)
                            client->sendGameOver();
                    }
                    world[x][y] = world[x][y + 1];
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
                    if(world[x][y] == 10){
                        tiles[x][y].setColor(Color::White);
                        tiles[x][y].setTexture(textureBlocked);
                    }else {
                        tiles[x][y].setColor(PIECE_COLOR[world[x][y]-1]);
                        tiles[x][y].setTexture(texture);
                    }
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

        if(blockSenderTimer>0) {
            text.setColor(Color::Red);
            text.setPosition(20, 60);
            text.setString(client->getBlockSender()+" sent a line!");
            renderer.draw(text);
            text.setColor(Color::White);
        }

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

            if(client == NULL) {
                text.setString("Press ENTER to play again");
                renderer.draw(text);
            }else {
                text.setString("Waiting for other players");
                renderer.draw(text);
                text.setPosition(75,900);
                text.setString("ENTER to chill singleplayer");
                renderer.draw(text);
            }


        }

    }

    RenderTexture * getRenderTexture(){
        renderer.display();
        return &renderer;
    }
};