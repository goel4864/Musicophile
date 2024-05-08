#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h> 
#include <iostream>
#include <cstring>
#include <thread>
#include "server.h"
#include "../network.h"

using namespace std;
using namespace MusicStreamingNetwork;

const int PORT =8070;

int ServerWaitForClientToConnect(int theSocket)
{
        int socketForClient = 0;
        socketForClient = accept(theSocket, (struct sockaddr*)NULL, NULL);
        return socketForClient;
}

int CreateServerSocketandListen(){

	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Bind socket to IP and port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);  //Convert to BIg endian (Network byte order)
    
	//reinterpret_cast<sockaddr*>(&serverAddress) use to convert sock_adddr_in to sock_addr structure 
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    // Listen which specifies the maximum length of the queue of pending connections.
	// It determines how many incoming connections can be waiting to be accepted by the server socket at any given time.
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listen failed\n";
        return 1;
    }
	return serverSocket;

}


int main(int argc, char** argv) 
{
     /*-----------------------------------------------------------------------
                        CREATE THE SERVER SOCKET AND LISTENING 
    -----------------------------------------------------------------------*/
	int serverSocket = CreateServerSocketandListen();

    MusicStreamer musicStreamer;
    int countClient = 0;

    while (true) {

    cout << " SERVER : Waiting for clients to join on port 8070... (serverSocket: " << serverSocket << ")" << endl;
    int socketForClient = ServerWaitForClientToConnect(serverSocket); 
    countClient++;

    cout << " SERVER : Client " << countClient << " joined at socket: " << socketForClient << endl;
    thread clientThread(&MusicStreamer::HandleClient, &musicStreamer, ref(socketForClient));    
    clientThread.detach(); 
    
    }

    std::cout << " SERVER : Server ended." << std::endl;
    close(serverSocket); 

	return 0;
}

