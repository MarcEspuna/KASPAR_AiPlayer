#include "AiPlayer.h"
#include <iostream>
#include <cmath>
#include <chrono>
#include <mutex>


static std::mutex mx;

AiPlayer::AiPlayer()
	: cursor({ 0, 0.0f,0.0f }), overObject(0), usrKey(false), enable(false), usrSelect(false)
{
	waitChild = new std::thread{ &AiPlayer::childWaiting, this };
	waitChild->detach();
	delete waitChild;
}

AiPlayer::~AiPlayer()
{
	enable = true;
	Sleep(300);
}

AiDesicion AiPlayer::makeDesicion()
{
	AiDesicion desicion = { 'S', 'S', 'N', targetObject.id + '0' };

	if (m_Objects.size() == 0)
		return desicion;

	float minDistance = 1000;				//All objects should be closer to that distance;

	if (enable)	// We set the new target
	{
		for (const auto& object : m_Objects)
		{
			float distance = calcDistance(cursor, object);
			if (minDistance > distance)
			{
				minDistance = distance;
				targetObject = object;
			}
		}
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
	}
	else {
		rInterface.figureNotErased();
	}

	return desicion;
}

void AiPlayer::processData(char* data, int size)
{
	if (data[0] == 'O')
		updateObjects(data, size);
	if (data[0] == 'C')
		updateCursor(data, size);
}

void AiPlayer::updateObjects(char* data, int len)
{
	enable = false;
	size_t nObjects = m_Objects.size();
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
	if (nObjects != m_Objects.size())
	{
		waitChild = new std::thread{ &AiPlayer::childWaiting, this };
		waitChild->detach();
		delete waitChild;
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
		if (overObject && overObject == targetObject.id)
		{
			rInterface.overFigure();
		}
		else 
		{
			rInterface.notOverFigure();
		}
		if (data[ptrIndex++] == 'Y')
		{
			usrKey = true;
		}
		else
		{
			usrKey = false;
		}
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
	else
		std::cout << "[AiPlayer]: ERROR, message length not match" << std::endl;

}

float AiPlayer::calcDistance(const Object& main, const Object& other)
{
	return std::sqrt((float)std::pow(main.x - other.x, 2) + (float)std::pow(main.y - other.y, 2));
}

void AiPlayer::childWaiting()
{
	mx.lock();
	//rInterface.unsetTargetObject();
	enable = false;
	size_t beforeObjects = m_Objects.size();
	auto startingTime = std::chrono::system_clock::now();
	auto endTime = std::chrono::system_clock::now();
	while (endTime - startingTime < std::chrono::duration<double>(15))
	{
		Sleep(250);
		if (enable) {
			mx.unlock();
			return;
		}
		endTime = std::chrono::system_clock::now();
	}
	enable = true;
	mx.unlock();
}
