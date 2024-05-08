#ifndef SERVER_H
#define SERVER_H
#include<mutex>
#include "../network.h"

using namespace std;
using namespace MusicStreamingNetwork;

class MusicStreamer {
public:
    void SendMusic(int socket, const string &songToPlay);
    void HandleClient(int &socketForClient);
};

#endif // SERVER_H
