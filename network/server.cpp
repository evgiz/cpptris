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
int num_connections = 0;
bool running = true;
TcpListener tcpListener;

Server::Server() {
    cout << "Starting server..." << endl;
    thread th(&Server::run, this);
    th.detach();
}

string Server::getName(int player) {
    if (player < num_connections)
        return connections[player]->getName();
    else
        return "";
}

void Server::sendLobbyData() {

    Packet pack;
    pack << (int)PACKET_TYPE_LOBBY;

    for (int i = 0; i < 4; i++) {
        if(i >= num_connections)
            pack << "";
        else pack << connections[i]->getName();
    }

    Server::sendAll(pack);

}

void Server::startGame(){
    Packet pack;
    pack << (int)PACKET_TYPE_START;
    Server::sendAll(pack);
}

void Server::sendAll(Packet packet) {
    for (int i = 0; i < num_connections; i++) {
        connections[i]->send(packet);
    }
}

void Server::sendAllExcept(int id, Packet packet) {
    for (int i = 0; i < num_connections; i++) {
        if(id == i)
            continue;
        connections[i]->send(packet);
    }
}

void Server::run() {

    if (tcpListener.listen((unsigned short) 31621) != Socket::Done) {
        cout << "Failed to start network." << endl;
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

        connections[num_connections] = new Connection(num_connections, this, socket);
        num_connections++;

    }

    cout << "Server got 4 clients! Stopping!" << endl;

}


