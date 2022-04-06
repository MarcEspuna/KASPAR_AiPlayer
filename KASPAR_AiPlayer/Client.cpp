#include "Client.h"
#include <iostream>

Client::Client()
	:connected(false)
{
	initWinsoc();
	initSocket();
}


Client::~Client()
{
	closesocket(s);
	std::cout << "[CLIENT]: Socket closed" << std::endl;
	WSACleanup();
	std::cout << "[CLIENT]: Winsocket closed" << std::endl;
}

void Client::connectS(const unsigned int& port, const char* address)
{
	server.sin_addr.s_addr = inet_addr(address);						//Connecting to localhost address
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	//Connect to remote server
	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		std::cout << "[CLIENT ERROR]: Error connecting to server" << std::endl;
		return;
	}
	connected = true;
	std::cout << "[CLIENT]: Successfully connected to server" << std::endl;
}

void Client::sendBuffer(const char* message)
{	
	send(s, message, (int)strlen(message), 0);
}

void Client::sendBuffer(const char* message, int len)
{
	send(s, message, len, (int)0);
}

int Client::recieveBuffer(char* reply, int maxSize)
{
	int size = 0;
	size = recv(s, reply, maxSize, 0);
	if (size < 0)
	{
		std::cout << "[CLIENT]: Error reading from socket, error code: " << size << std::endl;
		connected = false;
	}
	return size;
}

void Client::initWinsoc()
{
	std::cout << "[CLIENT]: Initialising Winsock Server...\n";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "[CLIENT ERROR]: Failed. Error Code : " << WSAGetLastError() << std::endl;

	}
	std::cout << "[CLIENT]: Initialised.\n";
}

void Client::initSocket()
{
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "[CLIENT ERROR]: Could not create socket : " << WSAGetLastError() << std::endl;
	}
	std::cout << "[CLIENT]: Socket created.\n";
}
