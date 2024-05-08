#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h> 
#include <signal.h> 
#include "server.h"

using namespace std;
using namespace MusicStreamingNetwork;


void MusicStreamer::SendMusic(int socket, const string &songToPlay) 
{
    string musicResName = "..//media/sfx//" + songToPlay;
    ifstream musicFile(musicResName.c_str(), ios::out | ios::binary);
    
    // get length of file
    musicFile.seekg(0, musicFile.end);
    int musicFileSize = musicFile.tellg();
    musicFile.seekg(0, musicFile.beg);

    cout << " SERVER : Sending music..." << endl;
    Packet musicStreamSize;
    musicStreamSize.m_packetType = MusicStreamingNetwork::packetType::PACKET_MUSIC_STREAM_SIZE;
    musicStreamSize.m_dataSize = sizeof(int);
    
    char dataLoadSize[MusicStreamingNetwork::s_packetMaxDataSize];
    musicStreamSize.m_data = dataLoadSize;
    
    memset(musicStreamSize.m_data, 0, MusicStreamingNetwork::s_packetMaxDataSize);
    memcpy(musicStreamSize.m_data, &musicFileSize, sizeof(int));

    bool packetSent = false;
    while (!packetSent) {
        packetSent = SendPacket(socket, musicStreamSize);
    }

    cout << " SERVER : Music size packet sent" << endl;
    cout << " SERVER : Music file size : " << musicFileSize<<endl;
    int musicDataSent = 0;
    char dataLoad[MusicStreamingNetwork::s_packetMaxDataSize];

    unsigned int nrMusicStreamPacksSent = 0;
    while (musicDataSent < musicFileSize) {
        int nrBytesRead = musicFile.readsome(dataLoad, MusicStreamingNetwork::s_packetMaxDataSize);

        Packet musicStream;
        musicStream.m_packetType = MusicStreamingNetwork::packetType::PACKET_MUSIC_STREAM;
        musicStream.m_dataSize = nrBytesRead;
        musicStream.m_data = dataLoad;

        bool packetSent = false;
        while (!packetSent) {
            packetSent = SendPacket(socket, musicStream);
        }

        musicDataSent += nrBytesRead;

        if (nrBytesRead <= 0) {
            cout << " SERVER : Error reading: " << nrBytesRead << endl;
        }
        nrMusicStreamPacksSent++;
    }

    musicFile.close();
    cout << " SERVER : Sent " << musicDataSent << "\n SERVER : Amount of data in " << nrMusicStreamPacksSent << " no of packets, whole file sent " << endl;
}

void MusicStreamer::HandleClient(int &socketForClient) {
    
    bool clientDisconnected = false;

    while (!clientDisconnected) {
        Packet clientPacket;
        if (MusicStreamingNetwork::ReceivePacket(socketForClient, clientPacket)) {
            switch (clientPacket.m_packetType) {
                case MusicStreamingNetwork::packetType::PACKET_END_CONNECTION:
                    clientDisconnected = true;
                    std::cout << " SERVER : Client sent end of session packet" << std::endl;
                    break;
                case MusicStreamingNetwork::packetType::PACKET_REQUEST_SONG:
                    std::string songRequested(clientPacket.m_data);
                    FreePacketData(clientPacket); 
                    SendMusic(socketForClient, songRequested);
                    break;

            }
        }
    }
    close(socketForClient);
}
