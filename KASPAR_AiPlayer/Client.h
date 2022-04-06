#pragma once
#include <stdio.h>
#include <winsock2.h>

class Client
{
public:
	Client();
	~Client();

	void connectS(const unsigned int& port, const char* address = "127.0.0.1");
	void sendBuffer(const char* message);
	void sendBuffer(const char* message, int len);
	int	 recieveBuffer(char* reply, int maxSize);
	inline bool isConnected() { return connected; }

private:
	WSADATA wsa;						//Winsocket
	SOCKET s;							//Actual socket
	sockaddr_in server;					//Struct used for: 
	bool connected;

	void initWinsoc();
	void initSocket();


};

