#pragma once
#include<stdio.h>
#include<winsock2.h>


class Server
{
public:

	Server();
	~Server();

	void bindS(const unsigned int& port, const unsigned long& address = INADDR_ANY);
	bool listenS();
	void sendBuffer(const char* message);
	void sendBuffer(const char* message, int len);
	unsigned int recieveBuffer(char* reply);
	void recursiveResieve();
	void stop();
	bool isActive();


private:
	bool active;
	WSADATA wsa;								//Winsocket
	SOCKET s, new_socket;						//Actual socket
	sockaddr_in server, client;					//Struct used for: 
					
	
	void initWinsoc();
	void initSocket();


};

