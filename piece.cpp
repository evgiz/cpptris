//
// Created by Sigve Røkenes on 25/10/2018.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "common.cpp"

using namespace std;
using namespace sf;

const static int PIECE_O[4][4]=
        {{0,0,0,0},
         {0,1,1,0},
         {0,1,1,0},
         {0,0,0,0}};

const static int PIECE_L[4][4]=
        {{0,1,0,0},
         {0,1,0,0},
         {0,1,1,0},
         {0,0,0,0}};

const static int PIECE_J[4][4]=
        {{0,0,1,0},
         {0,0,1,0},
         {0,1,1,0},
         {0,0,0,0}};

const static int PIECE_Z[4][4]=
        {{0,0,0,0},
         {0,1,1,0},
         {1,1,0,0},
         {0,0,0,0}};

const static int PIECE_ZZ[4][4]=
        {{0,0,0,0},
         {0,1,1,0},
         {0,0,1,1},
         {0,0,0,0}};

const static int PIECE_T[4][4]=
        {{0,0,0,0},
         {0,1,0,0},
         {1,1,1,0},
         {0,0,0,0}};

const static int PIECE_I[4][4]=
        {{0,1,0,0},
         {0,1,0,0},
         {0,1,0,0},
         {0,1,0,0}};

const static Color PIECE_COLOR[7] = {
        Color(255, 51, 0),
        Color(255, 255, 0),
        Color(0, 102, 255),
        Color(0, 255, 255),
        Color(204, 51, 255),
        Color(255, 153, 51),
        Color(0,255,0)
};

class Piece {

private:
    int type;

    Sprite sprites[4][4];
    int piece[4][4];

    int pos_x = 3;
    float pos_y = -3;
    int shadow_y = -3;

    float mov_right_timer = 0;
    float mov_left_timer = 0;
    float place_timer = 0;

    bool isPlaced = false;
    bool isDropping = false;
    float dropSpeed = 0;

    bool didReleaseUp = false;
    bool didReleaseSpace = false;

    bool gameOver = false;


public:
    Piece(){

        type = rand() % 7;

        switch(type){
            case 0:
                memcpy(piece, PIECE_O, sizeof(PIECE_O));
                break;
            case 1:
                memcpy(piece, PIECE_L, sizeof(PIECE_L));
                break;
            case 2:
                memcpy(piece, PIECE_J, sizeof(PIECE_J));
                break;
            case 3:
                memcpy(piece, PIECE_Z, sizeof(PIECE_Z));
                break;
            case 4:
                memcpy(piece, PIECE_ZZ, sizeof(PIECE_ZZ));
                break;
            case 5:
                memcpy(piece, PIECE_T, sizeof(PIECE_T));
                break;
            case 6:
                memcpy(piece, PIECE_I, sizeof(PIECE_I));
                break;
        }

        for(int y=0;y<4;y++){
            for(int x=0;x<4;x++){
                sprites[y][x].setColor(PIECE_COLOR[type]);
                sprites[y][x].setScale(TILE_SIZE/16.0f, TILE_SIZE/16.0f);
            }
        }
    }

    void update(float delta, float spd, int (&world)[10][20], Sprite (&tiles)[10][20]){

        int py = (int)pos_y;

        if(!isDropping) {
            if (Keyboard::isKeyPressed(Keyboard::Up)) {
                if (didReleaseUp) {
                    didReleaseUp = false;
                    rotate(world);
                }
            } else didReleaseUp = true;

            if (Keyboard::isKeyPressed(Keyboard::Left)) {
                if (mov_left_timer >= 0) {
                    mov_left_timer = -.1f;
                    if (place_free(pos_x - 1, py, world))
                        pos_x--;
                }
                mov_left_timer += delta;
            } else mov_left_timer = 0;

            if (Keyboard::isKeyPressed(Keyboard::Right)) {
                if (mov_right_timer >= 0) {
                    mov_right_timer = -.1f;
                    if (place_free(pos_x + 1, py, world))
                        pos_x++;
                }
                mov_right_timer += delta;
            } else mov_right_timer = 0;

            shadow_y = pos_y;
            while(place_free(pos_x, shadow_y+1, world)){
                shadow_y++;
            }

            if(Keyboard::isKeyPressed(Keyboard::Space)){
                if(didReleaseSpace) {
                    isDropping = true;
                }
            }else didReleaseSpace = true;
        }

        pos_y += delta * 3 * spd;

        if(isDropping) {
            pos_y += delta * dropSpeed;
            dropSpeed += delta * 500;
        }else if(Keyboard::isKeyPressed(Keyboard::Down))
            pos_y += delta * 9 * spd;

        if(!place_free(pos_x, py+1, world)){
            pos_y = shadow_y;
            place_timer += delta;

            if(place_timer > .4f || isDropping){
                isPlaced = true;
                fill_world(world, tiles);
            }
        }else place_timer = 0;
    }

    void rotate(int (&world)[10][20]) {

        // Rotate piece
        int pn[4][4];
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                pn[i][j] = piece[4 - j - 1][i];
            }
        }
        int tmp[4][4];
        memcpy(tmp, piece, sizeof(piece));
        memcpy(piece, pn, sizeof(piece));

        // Validate new position
        if(!place_free(pos_x, (int)pos_y, world)) {
            int d = pos_x>=5 ? 1 : -1;
            if(place_free(pos_x+d, (int)pos_y, world)){
                pos_x += d;
                return;
            }else if(place_free(pos_x-d, (int)pos_y, world)){
                pos_x -= d;
                return;
            }
            memcpy(piece, tmp, sizeof(piece));
        }

    }

    bool place_free(int px, int py, int (&world)[10][20]) {
        for(int x=0;x<4;x++){
            for(int y=0;y<4;y++){
                if(piece[y][x]) {
                    if(py+y < 0)continue;
                    if(px+x >= 10 || px+x < 0)
                        return false;
                    if(py+y >= 20) {
                        return false;
                    }
                    if (world[px + x][py + y] > 0)
                        return false;
                }
            }
        }
        return true;
    }

    void fill_world(int (&world)[10][20], Sprite (&tiles)[10][20]){
        for(int x=0;x<4;x++) {
            for (int y = 0; y < 4; y++) {
                if((int)pos_y+y < 0){
                    gameOver = true;
                    return;
                }
                if (piece[y][x]) {
                    if (pos_x + x >= 0 && pos_x + x < 10)
                        if ((int) pos_y + y >= 0 && (int)pos_y + y < 20) {
                            world[pos_x + x][(int) pos_y + y] = type+1;
                            tiles[pos_x + x][(int) pos_y + y] = sprites[y][x];
                        }
                }
            }
        }
    }

    void draw(RenderTexture * renderer, Texture texture){
        for(int y=0;y<4;y++){
            for(int x=0;x<4;x++){
                if(piece[y][x]) {
                    sprites[y][x].setTexture(texture);
                    if(!isPlaced){
                        Color col = PIECE_COLOR[type];
                        sprites[y][x].setColor(Color(col.r/3, col.g/3, col.b/3));
                        sprites[y][x].setPosition(Vector2f((pos_x + x) * TILE_SIZE, (shadow_y + y) * TILE_SIZE));
                        renderer->draw(sprites[y][x]);
                    }
                    sprites[y][x].setColor(PIECE_COLOR[type]);
                    sprites[y][x].setPosition(Vector2f((pos_x + x) * TILE_SIZE, (pos_y + y) * TILE_SIZE));
                    renderer->draw(sprites[y][x]);
                }
            }
        }
    }

    bool isPiecePlaced(){
        return isPlaced;
    }

    bool isGameOver(){
        return gameOver;
    }

};