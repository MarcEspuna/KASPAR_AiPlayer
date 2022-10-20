#include "Controller.h"
#include <iostream>

Controller::Controller()
{



}

Controller::~Controller()
{
	
}

void Controller::run()
{
	gInterface.addListener(&aiPlayer);
	gInterface.connect(8888);

}

void Controller::setFilnameLog(char* arg1, char* arg2)
{
	std::string logFilename;
	logFilename.append(arg1);
	logFilename.append("_");
	logFilename.append(arg2);
	logFilename.append(".log");
	aiPlayer.setLogfilename(logFilename);
}


