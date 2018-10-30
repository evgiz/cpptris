
#include <iostream>
#include <thread>
#include <SFML/Network.hpp>
#include <string>

#include "connection.h"
#include "server.h"
#include "../common.cpp"

using namespace std;
using namespace sf;

Connection *connections[4];

bool gameOver[4];
int gameOverCount = 0;
bool inGame = false;

int num_connections = 0;
bool running = true;
TcpListener tcpListener;

Server::Server() {
    cout << "Starting server..." << endl;
    thread th(&Server::run, this);
    th.detach();
}

string Server::getName(int player) {
    if (connections[player] != NULL)
        return connections[player]->getName();
    else
        return "";
}

void Server::sendLobbyData() {

    Packet pack;
    pack << (int)PACKET_TYPE_LOBBY;

    for (int i = 0; i < 4; i++) {
        if(connections[i] == NULL)
            pack << "";
        else
            pack << connections[i]->getName();
    }

    Server::sendAll(pack);

}

void Server::setGameOver(int id){

    gameOver[id] = true;

    int gameOverCount = 0;
    int winner = 0;

    for(int i=0;i<4;i++){
        if(gameOver[i] || connections[i]==NULL)
            gameOverCount++;
        else
            winner = i;
    }

    if(gameOverCount >= 3){

        for(int i=0;i<4;i++) {
            gameOver[id] = false;
        }

        inGame = false;

        Packet finPack;
        finPack << (int)PACKET_TYPE_FINISHGAME;
        finPack << winner;
        sendAll(finPack);
    }

}

void Server::startGame(){
    inGame = true;
    for(int i=0;i<4;i++)
        gameOver[i] = false;
    Packet pack;
    pack << (int)PACKET_TYPE_START;
    Server::sendAll(pack);
}

void Server::sendAll(Packet packet) {
    for (int i = 0; i < 4; i++) {
        if(connections[i] == NULL)
            continue;
        connections[i]->send(packet);
    }
}

void Server::sendAllExcept(int id, Packet packet) {
    for (int i = 0; i < 4; i++) {
        if(id == i || connections[i] == NULL)
            continue;
        connections[i]->send(packet);
    }
}

void Server::disconnect(int id){

    if(connections[id] == NULL)
        return;

    cout << "Client with id " << id << " disconnected." << endl;

    delete connections[id];
    connections[id] = NULL;
    num_connections--;

    if(inGame){
        setGameOver(id);
    }
    sendLobbyData();
}

void Server::run() {

    running = true;

    if (tcpListener.listen((unsigned short) 31621) != Socket::Done) {
        cout << "Failed to start server." << endl;
        running = false;
        return;
    }
    cout << "Server listening on port 31621" << endl;

    while (running) {

        if (num_connections == 4)
            continue;

        cout << "Listening for clients..." << endl;

        TcpSocket *socket = new TcpSocket;

        if (tcpListener.accept(*socket) != Socket::Done) {
            cout << "Accepting client failed." << endl;
            continue;
        }

        cout << "Client connected!" << endl;

        for(int i=0;i<4;i++) {
            if(connections[i] == NULL) {
                connections[i] = new Connection(i, this, socket);
                num_connections++;
                break;
            }
        }

    }

}

bool Server::isRunning(){
    return running;
}

void Server::stop(){
    running = false;
    for(int i=0;i<4;i++){
        if(connections[i] != NULL){
            connections[i]->stop();
        }
    }
    tcpListener.close();
    num_connections = 0;
}

