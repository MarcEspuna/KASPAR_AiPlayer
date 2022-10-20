#pragma once
#include "Structures.h"
#include <vector>
#include <string>

class Listener
{
public:
	
	virtual AiDesicion makeDesicion() = 0;
	virtual void updateCursor(char* data, int size) = 0;
	virtual void updateObjects(char* data, int size) = 0;
	virtual void updateDescription(const std::vector<std::string>& descriptions) = 0;
	virtual void startGame() = 0;
	virtual void endGame() = 0;

private:






};