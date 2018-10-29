
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "common.cpp"
#include "keyboard.h"

#include "piece.h"

using namespace std;
using namespace sf;

Piece::Piece(){

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

void Piece::update(float delta, float spd, int (&world)[10][20]){

    int py = (int)pos_y;

    if(!isDropping) {
        if (KeyboardManager::keyDown(Keyboard::Up)) {
            rotate(world);
        }

        if (KeyboardManager::keyPressed(Keyboard::Left)) {
            if (mov_left_timer >= 0) {
                mov_left_timer = -.1f;
                if (place_free(pos_x - 1, py, world))
                    pos_x--;
            }
            mov_left_timer += delta;
        } else mov_left_timer = 0;

        if (KeyboardManager::keyPressed(Keyboard::Right)) {
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

        if(KeyboardManager::keyDown(Keyboard::Space)){
            isDropping = true;
        }
    }

    pos_y += delta * 3 * spd;

    if(isDropping) {
        pos_y += delta * dropSpeed;
        dropSpeed += delta * 500;
    }else if(KeyboardManager::keyPressed(Keyboard::Down))
        pos_y += delta * 9 * spd;

    if(!place_free(pos_x, py+1, world)){
        pos_y = shadow_y;
        place_timer += delta;

        if(place_timer > .4f || isDropping){
            isPlaced = true;
            fill_world(world);
        }
    }else place_timer = 0;
}

void Piece::rotate(int (&world)[10][20]) {

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

bool Piece::place_free(int px, int py, int (&world)[10][20]) {
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

void Piece::fill_world(int (&world)[10][20]){
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
                    }
            }
        }
    }
}

void Piece::draw(RenderTexture * renderer, Texture texture){
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

float Piece::getX(){
    return pos_x;
}
float Piece::getY(){
    return pos_y;
}
int * Piece::getBlock(){
    int * blocks = new int[4*4];
    for(int i=0;i<4*4;i++){
        blocks[i] = piece[i/4][i%4] ? type+1 : 0;
    }
    return blocks;
}

bool Piece::isPiecePlaced(){
    return isPlaced;
}

bool Piece::isGameOver(){
    return gameOver;
}

