/*--Author：李宏杰--*/
#include "clientManager.h"

//日志系统
#include "logger.h"

namespace clientManagerNS
{
	logger *myLogger = logger::getInstance();
};
using namespace clientManagerNS;

enum OpenCVFactor
{
	maxFrameRate = cameraCaptureRate,
	minFrameRate = 1,
	maxScaleFactor = 3,
	minScaleFactor = 0,
};

clientManager *clientManager::instance = new clientManager;

clientManager::clientManager()
{

}

clientManager * clientManager::getInstance()
{
	return instance;
}

/*
	函数：添加客户端信息，返回是否成功
	首先查询是否已存在，若不存在则插入
*/
bool clientManager::addClient(unsigned long session, SOCKET socket, unsigned int port, bool enableUDP)
{
	PerClientData client;

	client.socket = socket;
	client.streamingPort = port;
	client.enableUDP = enableUDP;

	/*
		为了摄像头准备的
	*/

	client.scaleFactorX = 1;
	client.scaleFactorY = 1;
	client.play = false;
	client.frameRate = cameraCaptureRate / 2;  //使用摄像机固定采集率的一半作为客户端初始帧率

	//Additional attribute
	client.threshold = false;
	client.isDrawn = false;
	client.axesX = 20;
	client.axesY = 20;

	if (clientList.find(session) == clientList.end())
	{
		clientList.insert(make_pair(session, client));

		socToSessList.insert(make_pair(socket, session));

		return true;
	}

	//607,Can't add new client, client duplicated.
	myLogger->logError(607);

	return false;
}

/*
	函数：查询客户端是否存在，返回是否存在
*/
bool clientManager::searchClient(unsigned long session)
{
	auto iter = clientList.find(session);

	if (iter != clientList.end())
	{
		return true;
	}

	return false;
}

/*
	函数：查询客户端是否存在，返回是否存在
*/
bool clientManager::searchClient(SOCKET socket)
{
	auto iter = socToSessList.find(socket);

	if (iter != socToSessList.end())
	{
		return true;
	}

	return false;
}

/*
	函数：获取客户端信息，返回是否成功（换句话说是是否存在）
*/
bool clientManager::getClientInfo(unsigned long session, SOCKET &socket, unsigned int &port, bool &enableUDP)
{
	auto iter = clientList.find(session);

	if (iter != clientList.end())
	{
		socket = iter->second.socket;

		port = iter->second.streamingPort;

		enableUDP = iter->second.enableUDP;

		return true;
	}

	//606,Can't get client info from client manager, no such client in lists.
	myLogger->logError(606);

	return false;
}

/*
	函数：删除客户端信息，返回是否成功
*/
bool clientManager::removeClient(unsigned long session)
{
	auto iter = clientList.find(session);

	if (iter != clientList.end())
	{
		auto iterator = socToSessList.find(iter->second.socket);

		clientList.erase(iter);

		socToSessList.erase(iterator);

		return true;
	}

	//Error: 605,Can't delete client in client manager, no such client in lists.
	myLogger->logError(605);

	return false;
}

map<unsigned long, PerClientData>::iterator clientManager::getIteratorStart()
{
	return clientList.begin();
}

map<unsigned long, PerClientData>::iterator clientManager::getIteratorEnd()
{
	return clientList.end();
}

bool clientManager::changePlayFactor(SOCKET index)
{
	if (!searchClient(index))
	{
		//Error: 602,Can't find client while changing client info.
		myLogger->logError(602);

		return false;
	}

	auto key = socToSessList.find(index);
	auto iter = clientList.find(key->second);

	iter->second.play = !iter->second.play;

	return true;
}

bool clientManager::changePlayFactor(SOCKET index, double scaleFactor, bool changeX, bool changeY)
{
	if (!searchClient(index))
	{
		//Error: 602,Can't find client while changing client info.
		myLogger->logError(602);

		return false;
	}

	if (scaleFactor > maxScaleFactor || scaleFactor < minScaleFactor)
	{
		//Error: 603,Scale factor limit exceeded while changing client info.
		myLogger->logError(603);

		return false;
	}

	auto key = socToSessList.find(index);
	auto iter = clientList.find(key->second);

	if(changeX)iter->second.scaleFactorX = scaleFactor;
	if(changeY)iter->second.scaleFactorY = scaleFactor;

	return true;
}

bool clientManager::changePlayFactor(SOCKET index, int frameRateOffset)
{
	if (!searchClient(index))
	{
		//Error: 602,Can't find client while changing client info.
		myLogger->logError(602);

		return false;
	}

	auto key = socToSessList.find(index);
	auto iter = clientList.find(key->second);

	unsigned int frameRate = iter->second.frameRate + frameRateOffset;

	if (frameRate > maxFrameRate || frameRate < minFrameRate)
	{
		//Error: 604,Framerate factor limit exceeded while changing client info.
		myLogger->logError(604);

		return false;
	}
	
	iter->second.frameRate = frameRate;

	return true;
}

bool clientManager::changeImageFactor(SOCKET index)
{
	if (!searchClient(index))
	{
		//Error: 602,Can't find client while changing client info.
		myLogger->logError(602);

		return false;
	}

	auto key = socToSessList.find(index);
	auto iter = clientList.find(key->second);

	iter->second.threshold = !iter->second.threshold;

	return true;
}

bool clientManager::drawEllipse(SOCKET index)
{
	if (!searchClient(index))
	{
		//Error: 602,Can't find client while changing client info.
		myLogger->logError(602);

		return false;
	}

	auto key = socToSessList.find(index);
	auto iter = clientList.find(key->second);

	iter->second.isDrawn = !iter->second.isDrawn;

	return true;
}

bool clientManager::setEllipseX(SOCKET index, bool axesX)
{
	if (!searchClient(index))
	{
		//Error: 602,Can't find client while changing client info.
		myLogger->logError(602);

		return false;
	}

	auto key = socToSessList.find(index);
	auto iter = clientList.find(key->second);

	if (axesX)
	{
		if (iter->second.axesX < 150)iter->second.axesX += 5;
	}
	else
	{
		if (iter->second.axesX > 5)iter->second.axesX -= 5;
	}

	return true;
}

bool clientManager::setEllipseY(SOCKET index, bool axesY)
{
	if (!searchClient(index))
	{
		//Error: 602,Can't find client while changing client info.
		myLogger->logError(602);

		return false;
	}

	auto key = socToSessList.find(index);
	auto iter = clientList.find(key->second);

	if (axesY)
	{
		if (iter->second.axesY < 150)iter->second.axesY += 2;
	}
	else
	{
		if (iter->second.axesY > 5)iter->second.axesY -= 2;
	}

	return true;
}
