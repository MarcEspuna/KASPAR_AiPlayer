#pragma once
#include <thread>
#include "Server.h"
#include "Structures.h"
#include <string>
#include <chrono>
#include <vector>
#include <fstream>

class RobotInterface
{
public:
	RobotInterface();
	~RobotInterface();

	//X will be user for now
	void cursorLeft();
	void cursorRight();

	//Y will be AI for now
	void cursorUp();
	void cursorDown();

	//Stoped cursor
	void aiCursorStoped();
	void usrCursorStoped();

	void setTargetObject(const unsigned int& id);
	void figureErased(int objectId);
	void figureNotErased();
	void setRemainingFigures(const size_t& objectsLeft);
	void overFigure();
	void notOverFigure();
	void unsetTargetObject();
	void sendCurrentData();
	void sendDescription(const std::string& description);
	void endDescription();
	void transmition();

	// Start and reset commands
	void start();
	void restore();
	void endGame();

	void setLogFilename(std::string logFilename);

private:
	std::ofstream logFile;

	std::thread* connection;
	std::thread* pollingMessager;					// Sends the current state every a specified interval 
	std::thread* transmitionWorker;
	
	void connectionManager();
	void loadDefaultData();
	void refreshTimestamp();
	void constantMessaging();
	void txRequest();

	Server server;
	char data[14];
	bool enable;
};

