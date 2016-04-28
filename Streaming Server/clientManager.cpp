/*--Author������--*/
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
	��������ӿͻ�����Ϣ�������Ƿ�ɹ�
	���Ȳ�ѯ�Ƿ��Ѵ��ڣ��������������
*/
bool clientManager::addClient(unsigned long session, SOCKET socket, unsigned int port, bool enableUDP)
{
	PerClientData client;

	client.socket = socket;
	client.streamingPort = port;
	client.enableUDP = enableUDP;

	/*
		Ϊ������ͷ׼����
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
	��������ѯ�ͻ����Ƿ���ڣ������Ƿ����
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
	��������ȡ�ͻ�����Ϣ�������Ƿ�ɹ������仰˵���Ƿ���ڣ�
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
	������ɾ���ͻ�����Ϣ�������Ƿ�ɹ�
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
