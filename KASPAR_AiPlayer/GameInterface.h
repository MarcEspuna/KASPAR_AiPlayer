#pragma once
#include <thread>
#include <vector>
#include "Client.h"
#include "Structures.h"
#include "Listerner.h"

class GameInterface
{
public:
	GameInterface();
	~GameInterface();

	void reception();								//Main receiving function, it also notices if the server is disconnected 
	void transmition();								//Main transmit function
	void connect(const unsigned int& port, const char* address = "127.0.0.1");	//Connects the client to the server
	void addListener(Listener* l);					//Adds an interface where we will send the information to


private:
	Client client;									//Used to creat a socket to communicate with the server(main game)
	Listener* listener;								//Pointer to interface in order to let the aiPlayer know what to do
	std::thread* transmit;							//Thread that will be used to send communication to the game 
	std::thread* receive;							//Thread that will be used to receve communication from the game

	std::vector<std::string> getDescriptions(const char* data, uint8_t initSize);

};

