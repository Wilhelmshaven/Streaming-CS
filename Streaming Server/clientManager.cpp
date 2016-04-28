/*--Author：李宏杰--*/
#include "clientManager.h"

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

	client.scaleFactor = 1;
	client.play = false;
	client.frameRate = 10;

	if (clientList.find(session) == clientList.end())
	{
		clientList.insert(make_pair(session, client));

		socToSessList.insert(make_pair(socket, session));

		return true;
	}

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
	map<unsigned long, PerClientData>::iterator iter;

	iter = clientList.find(session);

	if (iter != clientList.end())
	{
		socket = iter->second.socket;

		port = iter->second.streamingPort;

		enableUDP = iter->second.enableUDP;

		return true;
	}

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

bool clientManager::changePlayFactor(SOCKET index, bool play)
{
	if (!searchClient(index))
	{
		//Todo: log error

		return false;
	}

	auto key = socToSessList.find(index);
	auto iter = clientList.find(key->second);

	iter->second.play = play;

	return true;
}

bool clientManager::changePlayFactor(SOCKET index, double scaleFactor)
{
	if (!searchClient(index))
	{
		//Todo: log error

		return false;
	}

	if (scaleFactor > maxScaleFactor || scaleFactor < minScaleFactor)
	{
		//Todo: log error

		return false;
	}

	auto key = socToSessList.find(index);
	auto iter = clientList.find(key->second);

	iter->second.scaleFactor = scaleFactor;

	return true;
}

bool clientManager::changePlayFactor(SOCKET index, int frameRateOffset)
{
	if (!searchClient(index))
	{
		//Todo: log error

		return false;
	}

	auto key = socToSessList.find(index);
	auto iter = clientList.find(key->second);

	unsigned int frameRate = iter->second.frameRate + frameRateOffset;

	if (frameRate > maxFrameRate || frameRate < minFrameRate)
	{
		//Todo: log error

		return false;
	}
	else
	{
		iter->second.frameRate = frameRate;
	}

	return true;
}
