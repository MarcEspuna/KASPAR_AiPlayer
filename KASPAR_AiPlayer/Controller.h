#pragma once
#include "AiPlayer.h"
#include "GameInterface.h"
#include "RobotInterface.h"

class Controller
{
public:
	
	Controller();
	~Controller();

	void run();

private:
	AiPlayer aiPlayer;
	GameInterface gInterface;




};

