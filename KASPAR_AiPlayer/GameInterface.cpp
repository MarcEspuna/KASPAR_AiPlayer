#include "GameInterface.h"
#include <iostream>
#include <cmath>


GameInterface::GameInterface()
	: transmit(nullptr), receive(nullptr), listener(nullptr)
{

}

GameInterface::~GameInterface()
{
	std::cout << "[GAME INTERFACE]: Closing communications..." << std::endl;
	if (transmit)
	{
		transmit->join();
		delete transmit;
		transmit = nullptr;
	}
	if (receive)
	{
		receive->join();
		delete receive;
		receive = nullptr;
	}
}


void GameInterface::reception()
{
	int size = 0;
	while (size >= 0)
	{
		char data[100];
		size = client.recieveBuffer(data, 100);
		if (size > 0 && listener)
		{
			listener->processData(data, size);
		}
	}
}

/* It will be responsible for transmiting the data regarding the decision that the AiPlayer has done */
void GameInterface::transmition()
{
	std::cout << "[AiPlayer]: Starting transmition" << std::endl;
	while (client.isConnected())
	{
		if (listener)
		{
			AiDesicion desicion = listener->makeDesicion();
			client.sendBuffer(&desicion.x, sizeof(desicion));
		}
		Sleep(10);
	}
	std::cout << "[AiPlayer]: Stoped transmition" << std::endl;
}

void GameInterface::connect(const unsigned int& port, const char* address)
{
	client.connectS(port, address);
	receive = new std::thread(&GameInterface::reception, this);
	transmit = new std::thread(&GameInterface::transmition, this);
}

void GameInterface::addListener(Listener* l)
{
	listener = l;
}


