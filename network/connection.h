
#ifndef CONNECTION_H
#define CONNECTION_H

#include "server.h"
#include <SFML/Network.hpp>

class Connection {

private:
    Server *server;
    sf::TcpSocket* socket;
    std::string username;
    int id;
    bool running;
public:
    Connection(int id, Server* server, sf::TcpSocket* socket);
    void send(sf::Packet packet);
    void run();
    void stop();
    std::string getName();
};

#endif //CONNECTION_H
