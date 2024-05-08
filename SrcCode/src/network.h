#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <vector>
#include <mutex>
#include <atomic>

using namespace std;

namespace MusicStreamingNetwork
{
    enum class packetType : int
    {
        PACKET_REQUEST_SONG,   
        PACKET_MUSIC_STREAM_SIZE,
        PACKET_MUSIC_STREAM,
        PACKET_END_CONNECTION
    };

    struct Packet
    {
        packetType      m_packetType;
        unsigned int    m_dataSize;
        char*           m_data;
    };

    struct NetworkTraffic
    {
	    std::vector<Packet> m_recv;
	    std::vector<Packet> m_send;

        // Mutexs are used to make it thread safe operating on the vectors of receive/send packets
	    std::mutex          m_recvMutex;
	    std::mutex 		    m_sendMutex;
    };

    constexpr int s_dataSendSize = 256; // Total packet size may never exceed 256 bytes, if so the packet will not be sent.
    constexpr int s_packetHeaderSize = sizeof(MusicStreamingNetwork::Packet);
    // constexpr int s_packetMaxDataSize = s_dataSendSize - s_packetHeaderSize;
    constexpr int s_packetMaxDataSize = s_dataSendSize - sizeof(Packet) + sizeof(char*);

    void    FreePacketData(Packet &packet);

    void    CreateRequestSongPacket(Packet &pack, const string &songName);

    bool    ReceivePacket(const int socket, Packet &packet);
    
    bool    SendPacket(const int socket, const Packet &packet);
    
    void    ShutDownConnection(const int socket);

    void    PutPacketOnSendQueue(const Packet &packet, NetworkTraffic &networkTraffic);

    void    HandlePackets(int &socket, std::atomic<bool> &clientRunning, NetworkTraffic &networkTraffic);
}

#endif