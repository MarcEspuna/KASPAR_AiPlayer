#include "Controller.h"

Controller::Controller()
	: aiPlayer(AiPlayer()), gInterface(GameInterface())
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

