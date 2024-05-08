#include <iostream>
#include <cstring>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "../network.h"
#include <atomic>
#include <mysql_connection.h> // MySQL database connection
#include <mysql_driver.h> // MySQL driver
#include <mysql_error.h> // MySQL error handling
#include <cppconn/statement.h> // MySQL statement
#include <cppconn/prepared_statement.h> // MySQL prepared statement
#include "audio.h"
#include "client.h"
#include "ui.h"


// Function prototypes
int connectClientToServer();
void handleDisconnect(ClientSessionData& clientSessionData, std::atomic<bool>& clientRunning);
void cleanupThreads(std::thread& threadHandlePackets, std::thread& threadProcessPackets);


bool readMySQLConfig(const std::string& filename, std::string& host, int& port, std::string& username, std::string& password) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open MySQL configuration file." << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("host=") != std::string::npos) {
            host = line.substr(5); // Skipping "host="
        } else if (line.find("port=") != std::string::npos) {
            port = std::stoi(line.substr(5)); // Skipping "port="
        } else if (line.find("username=") != std::string::npos) {
            username = line.substr(9); // Skipping "username="
        } else if (line.find("password=") != std::string::npos) {
            password = line.substr(9); // Skipping "password="
        }
    }

    file.close();
    return true;
}


int main() {


    /*-----------------------------------------------------------------------
                        CONNECTING TO MYSQL SERVER
    //-----------------------------------------------------------------------*/
	std::string host, username, password;
    int port;
	 if (!readMySQLConfig("..//src//mysql.config", host, port, username, password)) {
        return 1; // Error reading configuration file
    }
	
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement *pstmt;

    // Connect to the MySQL database
	try 
    {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(host + ":" + std::to_string(port), username, password);
        std::cout << "Connected to MySQL server!" << std::endl;
    }
    catch (sql::SQLException &e) 
    {
        std::cerr << "MySQL Error: " << e.what() << std::endl;
        return 1; // Error connecting to MySQL server
    }
   
    con->setSchema("user_authentication");


    /*-----------------------------------------------------------------------
                        SOCKET CREATION : Client Side
    //-----------------------------------------------------------------------*/

    ClientSessionData clientSessionData;
    std::atomic<bool> clientRunning = { true };
	int socketForClient = connectClientToServer();
    if (socketForClient == -1) {
        return 1; // Socket connection failed
    }

    /*-----------------------------------------------------------------------
                        CREATING THE IRRKLANG ENGINE 
    //-----------------------------------------------------------------------*/
    clientSessionData.m_streamingMusic.InitAudio();
	clientSessionData.m_songStarted = false;

     /*-----------------------------------------------------------------------
                        CREATING THE  THREADS 
    //-----------------------------------------------------------------------*/
    std::thread threadHandlePackets(MusicStreamingNetwork::HandlePackets, std::ref(socketForClient), std::ref(clientRunning), std::ref(clientSessionData.m_networkTraffic));//in network.cpp
    std::thread threadProcessPackets(&ClientSessionData::ProcessPacketsReceived, &clientSessionData, std::ref(clientRunning), std::ref(clientSessionData));// in client.cpp

    while (clientRunning) {
		UI::HandleUI(clientSessionData,socketForClient,con,pstmt,clientRunning);
    }

    handleDisconnect(clientSessionData, clientRunning);
    cleanupThreads(threadHandlePackets, threadProcessPackets);
	
	clientSessionData.m_streamingMusic.KillAudio();
	ShutDownConnection(socketForClient);

    return 0;

}


int connectClientToServer() {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }
	int port;
	std::cout<<" CLIENT : Enter server port no to connect : ";
    cin>>port;

	struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    if (connect(sock, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }
    return sock;

}


void handleDisconnect(ClientSessionData& clientSessionData, std::atomic<bool>& clientRunning) {
    Packet disconnect;
    disconnect.m_dataSize = 0;
    disconnect.m_packetType = MusicStreamingNetwork::packetType::PACKET_END_CONNECTION;
    PutPacketOnSendQueue(disconnect, clientSessionData.m_networkTraffic);

   bool disconnectPacketSent = false;
	while(!disconnectPacketSent)
	{
		std::lock_guard<std::mutex> guard(clientSessionData.m_networkTraffic.m_sendMutex);
		if(clientSessionData.m_networkTraffic.m_send.empty())
		{
			disconnectPacketSent = true;
			cout << "Informing server that we want to end session" << endl;
		}
	}

    clientRunning = false;
}

void cleanupThreads(std::thread& threadHandlePackets, std::thread& threadProcessPackets) {
    threadHandlePackets.join();
    threadProcessPackets.join();
}


