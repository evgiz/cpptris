
#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <SFML/Network.hpp>
#include "../piece.h"

class Client {

private:

    sf::TcpSocket socket;
    bool connected = false;

    std::string address;

    char username[25];
    int id;

    std::string * users = new std::string[4];
    int userWorlds[4][10*20];

    int addBlockCount = 0;
    int blockSender = 0;

    float userPiecePosition[4][2];
    int userPiece[4][4*4];

    bool gameOver[4];

    bool gameStarted = false;
    bool gameFinished = false;
    int gameWinner = 0;

    void connect();
    void send(sf::Packet packet);

public:
    Client(std::string name, std::string address);
    void resetState();
    void updateState(int (&world)[10][20]);
    void updatePieceState(Piece* piece);
    void sendGameOver();
    void sendBlock();
    bool isConnected();
    std::string getName(int i);
    bool isGameStarted();
    int* getUserWorld(int usr);
    float * getPiecePosition(int usr);
    int * getPiece(int usr);
    bool getGameOver(int usr);
    bool isGameFinished();
    int getGameWinner();
    bool addBlock();
    string getBlockSender();
    int getId();
};

#endif //CLIENT_H

