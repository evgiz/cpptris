//
// Created by Sigve Røkenes on 26/10/2018.
//

#include <iostream>
#include <thread>
#include <SFML/Network.hpp>
#include <string>

#include "connection.h"
#include "data.cpp"
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

        cout << "Server detected game over, only one player remains alive " << endl;

        inGame = false;

        Packet finPack;
        finPack << (int)PACKET_TYPE_FINISHGAME;
        finPack << winner;
        sendAll(finPack);
    } else
        cout << "Game over for " << id << ", " << num_connections-gameOverCount << " players remain" << endl;

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

    cout << "Server detected disconnect from " << id << endl;

    delete connections[id];
    connections[id] = NULL;
    num_connections--;

    if(inGame){
        cout << " ...ingame, setting game over " << endl;
        setGameOver(id);
    }
    sendLobbyData();
}

void Server::run() {

    running = true;

    if (tcpListener.listen((unsigned short) 31621) != Socket::Done) {
        cout << "Failed to start network." << endl;
        running = false;
        return;
    }
    cout << "Server listening on port 424842" << endl;

    while (running) {

        if (num_connections == 4)
            continue;

        cout << "Listening for clients..." << endl;

        TcpSocket *socket = new TcpSocket;

        if (tcpListener.accept(*socket) != Socket::Done) {
            cout << "Accepting client failed." << endl;
            continue;
        }
        cout << "Got client" << endl;

        for(int i=0;i<4;i++) {
            if(connections[i] == NULL) {
                connections[i] = new Connection(i, this, socket);
                num_connections++;
                break;
            }
        }

    }

    cout << "Server got 4 clients! Stopping!" << endl;

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

