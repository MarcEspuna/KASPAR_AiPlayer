#include "GameInterface.h"
#include <iostream>
#include <cmath>
#include <iostream>

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
	while (client.isConnected())
	{
		char data[100];
		size = client.recieveBuffer(data, 100);
		if (size > 0 && listener)
		{
			switch (data[0])
			{
			case 'I':		// Initializing game (Start game)
				listener->startGame();	
				listener->updateDescription(getDescriptions(data, size));
				break;

			case 'F':		// Finalizing game	(End game)
				listener->endGame();
				break;

			case 'O':
				listener->updateObjects(data, size);
				break;
			case 'C':
				listener->updateCursor(data, size);
				break;
			default:
				std::cout << "[GAME INTERFACE]: Error, received unsuported header through socket\n";
				std::cout << "size: " << size << std::endl;
				std::cout << "header: " << data[0] << std::endl;
			}
		}
	}
	std::cout << "Reception stoped\n";
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

/// <summary>
/// Receives a single buffer with all the descriptions seperated by a null character
/// </summary>
/// <param name="data"></param>
/// <param name="initSize"></param>
/// <returns></returns>
std::vector<std::string> GameInterface::getDescriptions(const char* data, uint8_t initSize)
{
	std::string strToAdd;
	std::vector<std::string> descriptions;
		
	// We copy all the received bytes in the current bloc
	uint8_t bufferSize = data[1];
	char* buffer = new char[bufferSize + 3];		// +3 to avoid copying outside array on while loop
	_memccpy(buffer, &data[2], 0, initSize - 2);		// Copying current reception

	std::cout << "desc size: " << (int)bufferSize << std::endl;
	std::cout << "init size: " << (int)initSize << std::endl;
	// We recursively copy the inside the buffer the remainning bytes
	while (initSize-2 < bufferSize)	// taking into account the header and size bytes
	{
		initSize += client.recieveBuffer(&buffer[initSize-1], bufferSize + 2 - initSize);
		std::cout << "[GAME INTERFACE]: Recursive recieved, not all the bytes received yet" << std::endl;
	}
	std::cout << "next init size: " << (int)initSize << std::endl;

	strToAdd.clear();
	for (int i = 0; i < bufferSize; i++) 	// While the ending hasn't been reached
	{
		if (buffer[i] != '#') strToAdd.push_back(buffer[i]);
		else {
			std::cout << "[GAME INTERFACE]: Received: " << strToAdd << "with size: " << strToAdd.size() << std::endl;
			descriptions.push_back(strToAdd);
			strToAdd.clear();
		}
	}

	delete[] buffer;						// Deleting auxiliar dynamic memory
	return descriptions;
}


