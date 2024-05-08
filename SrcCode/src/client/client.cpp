#include "client.h"
#include <iostream>
#include<cstring>


void ClientSessionData::HandleMusicStreamPacket(ClientSessionData &clientSessionData, const Packet &musicStreamPacket)
{
    clientSessionData.m_streamingMusic.FetchAudioToMemory(musicStreamPacket.m_data, musicStreamPacket.m_dataSize);
}



void ClientSessionData::HandleMusicStreamSizePacket(ClientSessionData &clientSessionData, const Packet &musicStreamSizePacket)
{
    int musicFileSize = 0;
    memcpy(&musicFileSize, musicStreamSizePacket.m_data, musicStreamSizePacket.m_dataSize);
    clientSessionData.m_streamingMusic.StartFetchAudio(musicFileSize);
}



void ClientSessionData::ProcessPacketsReceived(std::atomic<bool> &clientRunning, ClientSessionData &clientSessionData)
{
    while (clientRunning)
    {
        std::lock_guard<std::mutex> guard(clientSessionData.m_networkTraffic.m_recvMutex);
        while (!clientSessionData.m_networkTraffic.m_recv.empty())
        {
                Packet &thePacket = clientSessionData.m_networkTraffic.m_recv.front();
                if (thePacket.m_packetType == packetType::PACKET_MUSIC_STREAM_SIZE)
                {
                    clientSessionData.HandleMusicStreamSizePacket(clientSessionData, thePacket);
                    FreePacketData(thePacket);
                    clientSessionData.m_networkTraffic.m_recv.erase(clientSessionData.m_networkTraffic.m_recv.begin());
                }
                else 
                    if (thePacket.m_packetType == packetType::PACKET_MUSIC_STREAM)
                    {
                        clientSessionData.HandleMusicStreamPacket(clientSessionData, thePacket);
                        FreePacketData(thePacket);
                        clientSessionData.m_networkTraffic.m_recv.erase(clientSessionData.m_networkTraffic.m_recv.begin());
                        if (clientSessionData.m_streamingMusic.ReadyToPlayMusic() && !clientSessionData.m_songStarted)
                        {
                            clientSessionData.m_streamingMusic.StartMusic();
                            clientSessionData.m_songStarted = true;
                        }
                    }
        }
    }
}