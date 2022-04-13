#include "AiPlayer.h"
#include <iostream>
#include <cmath>
#include <chrono>
#include <mutex>

#define OBJECT_SIZE 40

static std::condition_variable mx_wait;
static std::mutex mx;

AiPlayer::AiPlayer()
	: overObject(0), usrKey(false), enable(false), usrSelect(false), waitChild(nullptr), cWaitting(true), cContinue(false)
{

}

AiPlayer::~AiPlayer()
{
	cWaitting = false;
	cContinue = true;
	mx_wait.notify_all();
}

AiDesicion AiPlayer::makeDesicion()
{
	AiDesicion desicion = { 'S', 'S', 'N', targetObject.id + '0' };

	if (m_Objects.size() == 0)
		return desicion;

	if (enable)	// We set the new target
	{
		for (const auto& object : m_Objects)
		{
			if (isInline(cursor, object)) 
			{
				targetObject = object;
				break;
			}

		}
		startingTime = std::chrono::system_clock::now();		//We reset the starting time of the waiting thread
		rInterface.setTargetObject(targetObject.id);
		enable = false;
	}

	// If the target is set we move the cursor. If not the cursor is stoped.
	if (targetObject.id != 0) {
		if (targetObject.y - cursor.y > 10.0f)
		{
			desicion.y = 'U';
			rInterface.cursorUp();
		}
		else if (targetObject.y - cursor.y < -10.0f)
		{
			desicion.y = 'D';
			rInterface.cursorDown(); 
		}
		else
			rInterface.aiCursorStoped();
		if (overObject == targetObject.id)
			desicion.hit = 'Y';
	}
	else {
		rInterface.aiCursorStoped();
	}
	//We only calculate the y axis right now 

	if (overObject && usrKey) {
		rInterface.figureErased(overObject);		
		rInterface.unsetTargetObject();
		targetObject.id = 0;
		cContinue = true;
		mx_wait.notify_one();					// We notify the waitting thread in order to start waiting again.
	}
	else {
		rInterface.figureNotErased();
	}
	return desicion;
}

void AiPlayer::processData(char* data, int size)
{
	static bool firstTime = true;

	if (data[0] == 'O')
		updateObjects(data, size);
	if (data[0] == 'C')
		updateCursor(data, size);

	// Implementation for now: (should be a better way to do this)
	if (firstTime)
	{
		waitChild = new std::thread{ &AiPlayer::childWaiting, this };
		waitChild->detach();
		delete waitChild;
		waitChild = nullptr;
		firstTime = false;
	}
}

void AiPlayer::updateObjects(char* data, int len)
{
	enable = false;
	size_t nObjects = m_Objects.size(); 
	size_t lastid = getLastId();	
	m_Objects.clear();
	int ptrIndex = 1;
	while (ptrIndex < len)
	{
		Object object;
		std::memcpy(&object.id, &data[ptrIndex], sizeof(object.id));
		ptrIndex += sizeof(object.id);
		std::memcpy(&object.x, &data[ptrIndex], sizeof(object.x));
		ptrIndex += sizeof(object.x);
		std::memcpy(&object.y, &data[ptrIndex], sizeof(object.y));
		ptrIndex += sizeof(object.y);
		m_Objects.push_back(object);
	}
	std::cout << "NEW OBJECTS RECEVED: " << std::endl;
	for (const auto& object : m_Objects)
	{
		std::cout << "Object id: " << object.id << " position (" << object.x << ", " << object.y << ") " << std::endl;
	}

	rInterface.setRemainingFigures(m_Objects.size());

	// Since reception is a lot faster than processing we need to do this workaround in order to unset the target when all objects are destroyed.
	if (!m_Objects.size() && lastid)	
	{
		rInterface.figureErased(lastid);
		rInterface.unsetTargetObject();
		targetObject.id = 0;
		cContinue = true;
		mx_wait.notify_one();					// We notify the waitting thread in order to start waiting again.
	}

}

void AiPlayer::updateCursor(char* data, int len)
{
	if (len == 12)
	{
		int ptrIndex = 1;
		{	//Here we check if the usr is moving up, down or right
			float newXPos;
			std::memcpy(&newXPos, &data[ptrIndex], sizeof(cursor.x));
			if (cursor.x > newXPos)
				rInterface.cursorLeft();
			else if (cursor.x < newXPos)
				rInterface.cursorRight();
			else
				rInterface.usrCursorStoped();
		}

		std::memcpy(&cursor.x, &data[ptrIndex], sizeof(cursor.x));
		ptrIndex += sizeof(cursor.x); //We update the new cursor x position.
		
		std::memcpy(&cursor.y, &data[ptrIndex], sizeof(cursor.y));
		ptrIndex += sizeof(cursor.y);
		overObject = data[ptrIndex++];
		
		if (overObject && overObject == targetObject.id)	rInterface.overFigure();
		else												rInterface.notOverFigure();
		
		if (data[ptrIndex++] == 'Y')						usrKey = true;
		else												usrKey = false;
		
		if (data[ptrIndex++] == 'Y')		// User select key
		{
			if (!usrSelect) {
				enable = true;
				usrSelect = true;
			}
		}
		else
		{
			usrSelect = false;	
		}
		//rInterface.figureNotErased();
	}
	else {
		std::cout << "[AiPlayer]: ERROR, message length not match" << std::endl;
	}

}

float AiPlayer::calcDistance(const Object& main, const Object& other)
{
	return std::sqrt((float)std::pow(main.x - other.x, 2) + (float)std::pow(main.y - other.y, 2));
}

bool AiPlayer::isInline(const Object& main, const Object& other)
{
	return std::abs(main.x - other.x) < OBJECT_SIZE;
}

/// <summary>
/// This thread is used to wait 15 sec (if the user selects an object we reset the timer) before automatically chosing a target.
/// </summary>
void AiPlayer::childWaiting()
{
	std::unique_lock<std::mutex> lock(mx);
	while (cWaitting)
	{
		std::cout << "[WAITTING THREAD]: counting waiting...\n";
		startingTime = std::chrono::system_clock::now();
		endTime = std::chrono::system_clock::now();
		while (endTime - startingTime < std::chrono::duration<double>(15))
		{
			Sleep(250);
			endTime = std::chrono::system_clock::now();
		}
		targetObject = closestObject();		// The ai will choose the closest object
		enable = true;						// In order to set the target object
		// We make the thread wait here untill the main program notifies us to continue.
		cContinue = false;
		mx_wait.wait(lock, [&] {return cContinue; });
		cContinue = false;					// In order to make the thread wait again once it reaches wait.
	}
	std::cout << "[WAITTING THREAD]: exit \n";
}

/// <summary>
/// Used to get the last remaing object id. Edge case of solving not restoring the target object when all objects are destroyed.
/// </summary>
/// <returns></returns>
size_t AiPlayer::getLastId()
{
	if (m_Objects.size() == 1)
		return (size_t)m_Objects[0].id;
	return 0;
}

Object AiPlayer::closestObject()
{
	Object target = Object();
	float minDistance = 10000.0f;		// No objects will be further than that.
	for (auto& obj : m_Objects)
	{
		float distance = calcDistance(cursor, obj);
		if (minDistance > distance) 
		{
			target = obj;
			minDistance = distance;
		}
	}
	return target;
}
