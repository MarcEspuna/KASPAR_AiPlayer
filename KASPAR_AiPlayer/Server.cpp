#include "Server.h"
#include <mutex>
#include <thread>
#include <iostream>


Server::Server()
	: active(false)
{
	initWinsoc();
	initSocket();
}

Server::~Server()
{
	if (active)
	{
		closesocket(s);
		std::cout << "[SERVER]: Closed socket" << std::endl;
		WSACleanup();
		std::cout << "[SERVER]: Closed winsocket" << std::endl;
	}
}

void Server::bindS(const unsigned int& port, const unsigned long& address)
{
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = address;
	server.sin_port = htons(port);

	//Bind
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		std::cout << "[SERVER ERROR]: Bind failed with error code : " << WSAGetLastError() << std::endl;
	}
	std::cout << "[SERVER]: Bind done.\n";
}

bool Server::listenS()
{
	listen(s, 3);
	std::cout << "[SERVER]: Waiting for incoming connections..." << std::endl;
	int c = sizeof(struct sockaddr_in);
	new_socket = accept(s, (struct sockaddr*)&client, &c);
	if (new_socket == INVALID_SOCKET)
	{
		printf("[SERVER]: Accept failed with error code : %d", WSAGetLastError());
		return false;
	}
	std::cout << "[SERVER]: Connection accepted" << std::endl;
	return true;
}
	
static std::mutex txMutex;

void Server::sendBuffer(const char* message)
{
	std::lock_guard<std::mutex> lock(txMutex);			// Only one thread is allowed in this function
	//Reply to client
	send(new_socket, message, (int)strlen(message), 0);
}

void Server::sendBuffer(const char* message, int len)
{
	std::lock_guard<std::mutex> lock(txMutex);			// Only one thread is allowed in this function
	//Reply to client
	send(new_socket, message, len, 0);

}

unsigned int Server::recieveBuffer(char* reply)
{
	int size = 0;
	while (!size)
	{
		size = recv(new_socket, reply, 14, 0);
	}
	reply[size] = '\0';
	return size;
}

void Server::recursiveResieve()
{
	char data[15];
	while (true)
	{
		unsigned int size = recv(new_socket, data, 15, 0);
		if (size > 0)
		{
			double timestamp;
			std::memcpy(&timestamp, data, 8);
			char* otherData;
			otherData = &data[8];
			std::cout << "Data recieved: " << " Timestamp: " << timestamp << "  data: " << otherData << std::endl;
		}
		
	}

}

void Server::stop()
{
	if (active)
	{
		closesocket(s);
		std::cout << "[SERVER]: Closed socket" << std::endl;
		WSACleanup();
		std::cout << "[SERVER]: Closed winsocket" << std::endl;
		active = false;
	}
}

bool Server::isActive()
{
	return active;
}

void Server::initWinsoc()
{
	std::cout << "[SERVER]: Initialising Winsock Server...\n";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "[SERVER ERROR]: Failed. Error Code : " << WSAGetLastError() << std::endl;
		return;
	}
	std::cout << "[SERVER]: Initialised.\n";
}

void Server::initSocket()
{
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "[SERVER ERROR]: Could not create socket : " << WSAGetLastError() << std::endl;
		active = false;
		return;
	}
	std::cout << "[SERVER]: Socket created.\n";
	active = true;
}