#pragma once
#include "Client.h"
#include "RobotInterface.h"
#include "Listerner.h"
#include <thread>
#include <vector>

class AiPlayer : public Listener
{
public:
	AiPlayer();
	~AiPlayer();
	AiDesicion makeDesicion();
	void processData(char* data, int size);

private:
	RobotInterface rInterface;
	std::vector<Object> m_Objects;
	Object cursor;
	bool usrKey;
	bool usrSelect;
	bool volatile enable;
	int overObject;

	void updateObjects(char* data, int len);
	void updateCursor(char* data, int len);
	float calcDistance(const Object& main, const Object& other);
	void childWaiting();

	std::thread* waitChild;
	Object targetObject = {0 , 0.0, 0.0};
};

