//
// Created by Sigve Røkenes on 26/10/2018.
//

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
            return;
        }

        int type;
        dPack >> type;

        cout << "Client got pack type " << type << endl;

        if(type == PACKET_TYPE_LOBBY){
            for(int i=0;i<4;i++){
                dPack >> users[i];
            }
        }else if(type == PACKET_TYPE_START){
            gameStarted = true;

        }else if(type == PACKET_TYPE_WORLD){

            int id = 0;
            int world[10*20];

            dPack >> id;

            for(int i=0;i<10*20;i++){
                dPack >> world[i];
            }

            cout << "Client got world: "<< endl;
            for(int y=0;y<20;y++){
                for(int x=0;x<10;x++){
                    if(world[x + y*10])
                        cout << "X";
                    else cout << "O";
                }
                cout << endl;
            }
            cout << endl;
            cout << "client got id " << id << endl;

            memcpy(userWorlds[id], world, sizeof(world));


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

    // Reset users
    for(int i=0;i<4;i++) {
        users[i] = "";
        for (int x = 0; x < 10 * 20; x++)
            userWorlds[i][x] = 0;
    }

    this->address = addr;

    cout << "Starting client with name " << name << endl;

    for(int i=0;i<4;i++){
        users[i] = "?";
    }

    memcpy(username, name.c_str(), 25);
    thread th (&Client::connect, this);
    th.detach();
}

void Client::updateState(int (&world)[10][20]){
    Packet packet;
    packet << (int)PACKET_TYPE_WORLD;
    for(int i=0;i<10*20;i++){
        packet << world[i%10][i/10];
    }
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



