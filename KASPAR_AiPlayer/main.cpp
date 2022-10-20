#include <iostream>
#include "Controller.h"

int main(int argc, char* argv[])
{

	Controller controller;
	if (argc == 3) controller.setFilnameLog(argv[1], argv[2]);
	controller.run();

}

