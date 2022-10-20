#pragma once
#include "Client.h"
#include "RobotInterface.h"
#include "Listerner.h"
#include <thread>
#include <vector>

using time_point = std::chrono::system_clock::time_point;

class AiPlayer : public Listener
{
public:
	AiPlayer();
	~AiPlayer();
	AiDesicion makeDesicion();
	void updateObjects(char* data, int len);
	void updateCursor(char* data, int len);
	void startGame();
	void updateDescription(const std::vector<std::string>& data);
	void endGame();
	void setLogfilename(std::string filename);

private:
	RobotInterface rInterface;
	std::vector<Object> m_Objects;
	Object cursor;
	bool usrKey;
	bool usrSelect;
	bool enable;
	int overObject;

	bool cWaitting;			// Will indicate that the waiting thread is running
	bool cContinue;			// Used to make the waitting thread to continue

	time_point startingTime;
	time_point endTime;



	float calcDistance(const Object& main, const Object& other);
	bool isInline(const Object& main, const Object& other);
	void initChildWaiting();
	void finiChildWaiting();
	void childWaiting();
	size_t getLastId();
	Object closestObject();

	std::thread* waitChild;
	Object targetObject;
};

