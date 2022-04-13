#include "RobotInterface.h"
#include <iostream>
#include <string>
#include <mutex>

#define TS 0				//Timestamp 8 bytes
#define RM 8				//Index data defines robot movement
#define PM 9				//Index data defines person movement
#define TO 10				//Index data defines target object
#define EF 11				//Erased figure
#define RF 12				//Remainning figures
#define OF 13				//Over figure

#define TX_DELAY 200		//Time waiting for new changes on every transmition request

static std::mutex m_txDelay;
static std::condition_variable s_txDelay;
static volatile bool sendData = false;

RobotInterface::RobotInterface()
{
	loadDefaultData();
	server.bindS(10000);
	connection = new std::thread{ &RobotInterface::connectionManager, this };		// Thread that manages reconnections
	pollingMessager = new std::thread{ &RobotInterface::constantMessaging, this};	// Thread that manages the 5s messaging
	transmitionWorker = new std::thread{ &RobotInterface::transmition, this };
}

RobotInterface::~RobotInterface()
{
	server.stop();
	if (connection)
	{
		connection->join();
		delete connection;
		connection = nullptr;
	}
	if (pollingMessager) 
	{
		pollingMessager->join();
		delete pollingMessager;
		pollingMessager = nullptr;
	}
	if (transmitionWorker) 
	{
		transmitionWorker->join();
		delete transmitionWorker;
		transmitionWorker = nullptr;
	}
}

void RobotInterface::connectionManager()
{
	while (server.listenS()){
		sendCurrentData();
	}
}


void RobotInterface::cursorLeft()
{
	refreshTimestamp();
	if (data[PM] != 'L')
	{
		data[PM] = 'L';
		txRequest();
	}
}

void RobotInterface::cursorRight()
{
	refreshTimestamp();
	if (data[PM] != 'R')
	{
		data[PM] = 'R';
		txRequest();
	}
}

void RobotInterface::cursorUp()
{
	refreshTimestamp();
	if (data[RM] != 'U')
	{
		data[RM] = 'U';
		txRequest();
	}
}

void RobotInterface::cursorDown()
{
	refreshTimestamp();
	if (data[RM] != 'D')
	{
		data[RM] = 'D';
		txRequest();
	}
}

void RobotInterface::aiCursorStoped()
{
	if (data[RM] != 'S')
	{
		data[RM] = 'S';
		txRequest();
	}
}

void RobotInterface::usrCursorStoped()
{
	refreshTimestamp();
	if (data[PM] != 'S')
	{
		data[PM] = 'S';
		txRequest();
	}
}

void RobotInterface::setTargetObject(const unsigned int& objId)
{
	refreshTimestamp();
	char id = objId + '0';
	if (data[TO] != id)
	{
		data[TO] = id;
		txRequest();
	}
}

void RobotInterface::unsetTargetObject()
{
	refreshTimestamp();
	if (data[TO] != '0')
	{
		data[TO] = '0';
		txRequest();
	}
}

void RobotInterface::sendCurrentData()
{
	server.sendBuffer(data, 14);
}

void RobotInterface::transmition()
{
	std::unique_lock<std::mutex> lock(m_txDelay);
	while (server.isActive()) 
	{
		s_txDelay.wait(lock, [&]() { return sendData; });
		Sleep(TX_DELAY);					
		server.sendBuffer(data, 14);
		sendData = false;
	}
}



//NOT REALLY NEEDED
void RobotInterface::figureErased(int objectId)
{
	refreshTimestamp();
	char id = objectId + '0';
	if (data[EF] != id)
	{
		data[EF] = id;
		txRequest();
	}
}

void RobotInterface::figureNotErased()
{
	refreshTimestamp();
	if (!sendData && data[EF] != '0')		// This way we guarantee to have notifed the erase figure (We wait a tx cycle)
	{
		data[EF] = '0';
		txRequest();
	}
}

void RobotInterface::setRemainingFigures(const size_t& objectsLeft)
{
	refreshTimestamp();
	std::string remaining = std::to_string(objectsLeft);
	if (data[RF] != remaining[0])
	{
		data[RF] = remaining[0];
		txRequest();
	}
}

void RobotInterface::overFigure()
{
	refreshTimestamp();
	if (data[OF] != 'Y')
	{
		data[OF] = 'Y';
		txRequest();
	}
}

void RobotInterface::notOverFigure()
{
	refreshTimestamp();
	if (data[OF] != 'N')
	{
		data[OF] = 'N';
		txRequest();
	}
}

void RobotInterface::loadDefaultData()
{
	refreshTimestamp();
	data[RM] = 'S';
	data[PM] = 'S';
	data[TO] = '0';
	data[EF] = '0';
	data[RF] = '5';
	data[OF] = 'N';
}

void RobotInterface::refreshTimestamp()
{
	double timestamp =
		(double)std::chrono::duration_cast<std::chrono::seconds>
		(std::chrono::system_clock::now().time_since_epoch()).count();
	std::memcpy(data, &timestamp, sizeof(timestamp));			//Copying timestamp to data buffer
}

/*
* Method used with an independent thread that sends the current state every 5 sec.
*/
void RobotInterface::constantMessaging()
{
	while (server.isActive())
	{
		Sleep(10000);
		//if (data[TO] == '0') {
			server.sendBuffer(data, 14);
		//}
	}
}

void RobotInterface::txRequest()
{
	if (!sendData) {
		sendData = true;
		s_txDelay.notify_one();
	}
}


