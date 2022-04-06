#pragma once
#include "Structures.h"

class Listener
{
public:
	
	virtual AiDesicion makeDesicion() = 0;
	virtual void processData(char* data, int size) = 0;
	

private:






};