
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

    running = true;

    thread th(&Connection::run, this);
    th.detach();

}

void Connection::send(Packet packet){
    if(socket->send(packet) != sf::Socket::Done){
        cout << "Server failed to send packet to " << username << endl;
        running = false;
        server->disconnect(id);
    }
}

void Connection::stop(){
    running = false;
    socket->disconnect();
}

void Connection::run() {
    cout << "Getting client name... " << endl;

    Packet namePacket;

    if (socket->receive(namePacket) != sf::Socket::Done) {
        cout << "Server couldnt get username from client " << endl;
        running = false;
        server->disconnect(id);
        return;
    }

    namePacket >> username;
    cout << "Server got name: " << username << endl;

    Packet idPacket;
    idPacket << id;
    send(idPacket);

    server->sendLobbyData();

    while(running){
        Packet packet;
        if(socket->receive(packet)!=Socket::Done){
            cout << "Failed to receive pack from client " << endl;
            running = false;
            server->disconnect(id);
            break;
        }

        int type;
        packet >> type;

        if(type == PACKET_TYPE_WORLD){

            int world[10*20];

            for(int i=0;i<10*20;i++){
                packet >> world[i];
            }

            Packet wPack;

            wPack << (int)PACKET_TYPE_WORLD;
            wPack << id;

            for(int i=0;i<10*20;i++)
                wPack << world[i];

            server->sendAllExcept(id, wPack);

        }else if(type == PACKET_TYPE_PIECE){

            float x,y;

            packet >> x;
            packet >> y;

            int piece[4*4];
            for(int i=0;i<4*4;i++) {
                packet >> piece[i];
            }

            Packet pPack;
            pPack << (int)PACKET_TYPE_PIECE;
            pPack << id << x << y;

            for(int i=0;i<4*4;i++)
                pPack << piece[i];

            server->sendAllExcept(id, pPack);

        }else if(type == PACKET_TYPE_BLOCK){
            Packet scPack;
            scPack << (int)PACKET_TYPE_BLOCK;
            scPack << id;
            server->sendAllExcept(id, scPack);
        }else if(type == PACKET_TYPE_GAMEOVER){
            Packet goPack;
            goPack << (int)PACKET_TYPE_GAMEOVER;
            goPack << id;
            server->sendAllExcept(id, goPack);
            server->setGameOver(id);
        }

    }

}

string Connection::getName(){
    return username;
}

