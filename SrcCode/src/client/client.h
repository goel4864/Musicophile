#ifndef CLIENT_H
#define CLIENT_H

#include "../network.h"
#include "audio.h"
#include <atomic>

using namespace MusicStreamingNetwork;

class ClientSessionData
{
	public:
	ClientSessionData() { m_songStarted = false;}

	std::string		m_serverName;
	bool			m_songStarted;
	StreamingMusic	m_streamingMusic;
	NetworkTraffic	m_networkTraffic;
	
	//Functions

	void ProcessPacketsReceived(std::atomic<bool> &clientRunning, ClientSessionData &clientSessionData);
	void HandleMusicStreamPacket(ClientSessionData &clientSessionData, const Packet &musicStreamPacket);
	void HandleMusicStreamSizePacket(ClientSessionData &clientSessionData, const Packet &musicStreamSizePacket);

	
};

#endif