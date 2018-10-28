//
// Created by Sigve Røkenes on 27/10/2018.
//

#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <SFML/Network.hpp>

class Client {

private:

    sf::TcpSocket socket;
    bool connected = false;

    std::string address;

    char username[25];
    int id;

    std::string * users = new std::string[4];
    int userWorlds[4][10*20];

    bool gameStarted = false;

    void connect();
    void send(sf::Packet packet);

public:
    Client(std::string name, std::string address);
    void updateState(int (&world)[10][20]);
    bool isConnected();
    std::string getName(int i);
    bool isGameStarted();
    int* getUserWorld(int usr);
    int getId();
};

#endif CLIENT_H

