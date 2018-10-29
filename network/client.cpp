
#include <string>
#include <iostream>
#include <thread>
#include <SFML/Network.hpp>

#include "data.cpp"
#include "client.h"
#include "../common.cpp"

using namespace sf;
using namespace std;

void Client::connect(){

    connected = true;

    if(socket.connect(this->address, 31621) != Socket::Done){
        cout << "Client failed to connect to server" << endl;
        connected = false;
        return;
    }

    cout << "Sending username to server... " << endl;
    Packet userPacket;
    userPacket << username;
    if(socket.send(userPacket) != Socket::Done){
        cout << "Failed to send username" << endl;
        connected = false;
        return;
    }

    Packet idPacket;
    if(socket.receive(idPacket) != Socket::Done){
        cout << "Failed to get ID froms server" << endl;
        connected = false;
        return;
    }
    idPacket >> id;
    cout << "Client started with ID " << id << endl;

    while(connected){

        Packet dPack;
        if (socket.receive(dPack) != sf::Socket::Done) {
            cout << "Server couldnt get pack from client " << endl;
            connected = false;
            return;
        }

        int type;
        dPack >> type;

        if(type == PACKET_TYPE_LOBBY){
            for(int i=0;i<4;i++){
                dPack >> users[i];
            }
        }else if(type == PACKET_TYPE_START){
            gameStarted = true;
            gameFinished = false;
        }else if(type == PACKET_TYPE_WORLD) {

            int id = 0;
            int world[10 * 20];

            dPack >> id;

            for (int i = 0; i < 10 * 20; i++) {
                dPack >> world[i];
            }

            cout << "Client got world from " << id << endl;
            memcpy(userWorlds[id], world, sizeof(world));

        }else if(type == PACKET_TYPE_PIECE) {

            int id;
            float x, y;

            dPack >> id;
            dPack >> x;
            dPack >> y;

            userPiecePosition[id][0] = x;
            userPiecePosition[id][1] = y;

            for (int i = 0; i < 4 * 4; i++)
                dPack >> userPiece[id][i];

        }else if(type == PACKET_TYPE_BLOCK){

            addBlockCount++;
            dPack >> blockSender;

        }else if(type == PACKET_TYPE_GAMEOVER){

            int id;
            dPack >> id;

            gameOver[id] = true;

        }else if(type == PACKET_TYPE_FINISHGAME){

            int winner;
            dPack >> winner;

            resetState();

            gameStarted = false;
            gameFinished = true;
            gameWinner = winner;

        }else {
            cout << "Client got unknown pack type " << type;
        }

    }

}

void Client::send(Packet packet){
    if(socket.send(packet) != Socket::Done){
        cout << "Failed to send pack from client " << endl;
    }
}


Client::Client(string name, string addr){

    for(int i=0;i<4;i++)
        users[i] = "";

    resetState();

    this->address = addr;

    cout << "Starting client with name " << name << endl;

    for(int i=0;i<4;i++){
        users[i] = "?";
    }

    memcpy(username, name.c_str(), 25);
    thread th (&Client::connect, this);
    th.detach();
}

void Client::resetState(){
    cout << "Client reset state " << endl;
    // Reset users
    for(int i=0;i<4;i++) {
        gameOver[i] = false;
        for (int x = 0; x < 10 * 20; x++)
            userWorlds[i][x] = 0;
        for(int x=0;x<4*4;x++)
            userPiece[i][x] = 0;
    }
    addBlockCount = 0;
}

void Client::updateState(int (&world)[10][20]){
    Packet packet;
    packet << (int)PACKET_TYPE_WORLD;
    for(int i=0;i<10*20;i++){
        packet << world[i%10][i/10];
    }
    cout << "Client sending world" << endl;
    send(packet);
}

void Client::updatePieceState(Piece* piece) {
    Packet packet;
    packet << (int)PACKET_TYPE_PIECE;
    packet << piece->getX();
    packet << piece->getY();

    int *block = piece->getBlock();
    for(int i=0;i<4*4;i++){
        packet << block[i];
    }

    send(packet);
}

void Client::sendGameOver() {
    Packet packet;
    packet << (int)PACKET_TYPE_GAMEOVER;
    send(packet);
}

void Client::sendBlock() {
    Packet packet;
    packet << (int)PACKET_TYPE_BLOCK;
    send(packet);
}

bool Client::isConnected(){
    return connected;
}

int Client::getId(){
    return id;
}

string Client::getName(int i){
    return users[i];
}

bool Client::isGameStarted(){
    return gameStarted;
}

int *Client::getUserWorld(int usr){
    return userWorlds[usr];
}

float * Client::getPiecePosition(int usr) {
    return userPiecePosition[usr];
}

int * Client::getPiece(int usr) {
    return userPiece[usr];
}
bool Client::getGameOver(int usr) {
    return gameOver[usr];
}

bool Client::addBlock() {
    if(addBlockCount>0){
        addBlockCount--;
        return true;
    }
    return false;
}

string Client::getBlockSender(){
    return users[blockSender];
}


bool Client::isGameFinished() {
    return gameFinished;
}

int Client::getGameWinner() {
    return gameWinner;
}

