//
// Created by Sigve Røkenes on 27/10/2018.
//

#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <SFML/Network.hpp>

class Server {
public:
    Server();
    std::string getName(int player);
    void sendLobbyData();
    void sendAll(sf::Packet packet);
    void sendAllExcept(int id, sf::Packet packet);
    void run();
    void startGame();
    void setGameOver(int id);
    bool isRunning();
};

#endif SERVER_H
