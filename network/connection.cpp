//
// Created by Sigve Røkenes on 27/10/2018.
//
#include <iostream>
#include <thread>
#include <SFML/Network.hpp>

#include "connection.h"
#include "../common.cpp"

using namespace sf;
using namespace std;

Connection::Connection(int id, Server* server, TcpSocket* socket){

    cout << "Starting client connection " << endl;

    this->id = id;
    this->server = server;
    this->socket = socket;

    thread th(&Connection::run, this);
    th.detach();

}

void Connection::send(Packet packet){
    if(socket->send(packet) != sf::Socket::Done){
        cout << "Server failed to send packet to " << username << endl;
    }
}

void Connection::run() {
    cout << "Getting client name... " << endl;

    Packet namePacket;

    if (socket->receive(namePacket) != sf::Socket::Done) {
        cout << "Server couldnt get username from client " << endl;
        return;
    }

    namePacket >> username;
    cout << "Server got name: " << username << endl;

    Packet idPacket;
    idPacket << id;
    send(idPacket);

    server->sendLobbyData();

    bool a = true;

    while(a){
        Packet packet;
        if(socket->receive(packet)!=Socket::Done){
            cout << "Failed to receive pack from client " << endl;
        }

        int type;
        packet >> type;

        if(type == PACKET_TYPE_WORLD){

            int world[10*20];

            for(int i=0;i<10*20;i++){
                packet >> world[i];
            }

            cout << "Server got world: "<< endl;
            for(int y=0;y<20;y++){
                for(int x=0;x<10;x++){
                    if(world[x + y*10])
                        cout << "X";
                    else cout << "O";
                }
                cout << endl;
            }
            cout << endl;

            Packet wPack;

            wPack << (int)PACKET_TYPE_WORLD;
            wPack << (int)id;

            for(int i=0;i<10*20;i++)
                wPack << world[i];

            server->sendAll(wPack);
        }

    }

}

string Connection::getName(){
    return username;
}

