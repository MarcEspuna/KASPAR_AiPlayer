#pragma once
#include "AiPlayer.h"
#include "GameInterface.h"
#include "RobotInterface.h"

class Controller
{
public:
	
	//Controller(std::string filename);
	Controller();
	~Controller();

	void run();
	void setFilnameLog(char* arg1, char* arg2);

private:
	AiPlayer aiPlayer;
	GameInterface gInterface;




};

