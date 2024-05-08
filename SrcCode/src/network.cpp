#include <sys/types.h>      // contains basic derived types
#include <arpa/inet.h>      // defines in_addr structure
#include <sys/socket.h>     // for socket creation
#include <netinet/in.h>     // contains constants and structures for internet domain addresses
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <atomic>
#include "network.h"        

namespace MusicStreamingNetwork
{
    void FreePacketData(Packet &packet)
    {
        if (packet.m_dataSize > 0)
        {
            delete[] packet.m_data;
        }
    }

    //----------------------------------------------------------------------

    bool SendPacket(const int socket, const Packet &packet)
    {
        char dataSend[s_dataSendSize]{};
        
        struct pollfd fds[1]{};
        fds[0].fd = socket;
        fds[0].events = POLLOUT;

        int ready = poll(fds, 1, 50); // Check if send would cause a block
        if (ready != -1 && (fds[0].revents & POLLOUT)) // Write on this socket will not cause a block
        {
            // Put thePacket into dataSend and send it
            size_t offset = 0;
            memcpy(dataSend + offset, &packet.m_packetType, sizeof(packet.m_packetType));
            offset += sizeof(packet.m_packetType);
            memcpy(dataSend + offset, &packet.m_dataSize, sizeof(packet.m_dataSize));
            offset += sizeof(packet.m_dataSize);
            if (packet.m_dataSize > 0) // Not all packets have a data load
            {
                memcpy(dataSend + offset, packet.m_data, MusicStreamingNetwork::s_packetMaxDataSize);
            }

            // Sending data
            int sendRetV = send(socket, dataSend, MusicStreamingNetwork::s_dataSendSize, 0);
            if (sendRetV == -1)
            {
                std::cerr << " ERROR ! Send error, errno:" << errno << std::endl;
            }

            return true;
        }
        return false;
    }

    //----------------------------------------------------------------------

    void ShutDownConnection(const int socket)
    {
        close(socket);
    }

    //----------------------------------------------------------------------

    bool ReceivePacket(const int socket, Packet &thePacket)
    {
        struct pollfd fds[1]{};
        fds[0].fd = socket;
        fds[0].events = POLLIN;

        int ready = poll(fds,1, 50); // Check if read would cause a block

        if (ready != -1 && (fds[0].revents & POLLIN) && ready != 0) // Read on this socket will not cause a block
        {
            unsigned int totalPacketSize = MusicStreamingNetwork::s_dataSendSize;
            char dataReceive[s_dataSendSize]{};

            // Read back the packet
            int sizeRead = read(socket, dataReceive, totalPacketSize);
            if (sizeRead > 0)
            {
                int curPos = sizeRead;
                while (sizeRead < totalPacketSize)
                {
                    sizeRead += read(socket, dataReceive + curPos, totalPacketSize - curPos);
                    curPos = sizeRead;

                    if (sizeRead == 0)
                    {
                        break;
                    }
                }

                if (sizeRead == totalPacketSize)
                {
                    // Transform the data we received into a packet struct
                    size_t offset = 0;
                    memcpy(&thePacket.m_packetType, dataReceive + offset, sizeof(thePacket.m_packetType));
                    offset += sizeof(thePacket.m_packetType);
                    memcpy(&thePacket.m_dataSize, dataReceive + offset, sizeof(thePacket.m_dataSize));
                    offset += sizeof(thePacket.m_dataSize);

                    if (thePacket.m_dataSize > 0) // Not all packets have a data load, for example PACKET_COVER_ART_END
                    {
                        thePacket.m_data = new char[thePacket.m_dataSize];
                        memcpy(thePacket.m_data, dataReceive + offset, thePacket.m_dataSize);
                    }

                    return true;
                }
                else // Something went wrong :(
                {
                    std::cerr << " ERROR ! Unexpected packet size: " << totalPacketSize << ", sizeRead: " << sizeRead << " Packet dropped." << std::endl;
                }
            }
        }

        return false;
    }

    //----------------------------------------------------------------------

    void HandlePackets(int &socket,std::atomic<bool> &clientRunning, NetworkTraffic &networkTraffic)
    {
        while (clientRunning)
        {
            // Receive packets
            Packet recivePacket;
            if (ReceivePacket(socket, recivePacket))
            {
                std::lock_guard<std::mutex> guard_readRecv(networkTraffic.m_recvMutex);
                networkTraffic.m_recv.push_back(recivePacket);
            }

            // Send packets
            std::lock_guard<std::mutex> guard_readSnd(networkTraffic.m_sendMutex);
            if (!networkTraffic.m_send.empty())
            {
                for (auto &packet : networkTraffic.m_send)
                {
                    bool packetSent = false;
                    while (!packetSent)
                    {  
                        packetSent = SendPacket(socket, packet);
                    }
                    FreePacketData(packet);
                }
                networkTraffic.m_send.clear();
            }
        }
    }

    //----------------------------------------------------------------------

    void PutPacketOnSendQueue(const Packet &packet, NetworkTraffic &networkTraffic)
    {
        std::lock_guard<std::mutex> guard(networkTraffic.m_sendMutex);
        networkTraffic.m_send.push_back(packet);
        cout<<endl;
    }

    //----------------------------------------------------------------------

    void CreateRequestSongPacket(Packet &pack, const std::string &songName)
    {
        pack.m_packetType = packetType::PACKET_REQUEST_SONG;
        pack.m_dataSize = songName.size() + 1; // We want the null-terminating character as well!
        pack.m_data = new char[MusicStreamingNetwork::s_dataSendSize]{};
        memcpy(pack.m_data, songName.c_str(), songName.size() + 1);
    }

} // namespace MusicStreamingNetwork
