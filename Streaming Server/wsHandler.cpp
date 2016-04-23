/*--Author£∫¿Ó∫ÍΩ‹--*/

#include "wsHandler.h"

namespace wsNS
{
	HANDLE hsWebMsgArrived = CreateSemaphore(NULL, 0, BUF_SIZE, NULL);

	HANDLE hsWebHandshake = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::webHandshake);
};

using namespace wsNS;

wsHandler *wsHandler::instance = new wsHandler;

queue<myWSMsg> wsHandler::msgInQueue, wsHandler::msgOutQueue;

wsHandler * wsHandler::getInstance()
{
	return instance;
}

void wsHandler::decodeMsg(SOCKET socket, string msg)
{
	myWSMsg tmp;

	tmp.index = socket;
	tmp.msg = msg;

	msgInQueue.push(tmp);

	ReleaseSemaphore(hsWebMsgArrived, 1, NULL);
}

bool wsHandler::getResponse(SOCKET & socket, string & msg)
{
	if (msgOutQueue.empty())
	{
		return false;
	}

	myWSMsg tmp = msgOutQueue.front();

	msgOutQueue.pop();

	socket = tmp.index;

	msg = tmp.msg;

	return true;
}



DWORD wsHandler::decodeThreadFunc(LPVOID lapram)
{
	myWSMsg message;

	string responseMsg;

	string srvLocation = "WebSocket-Location: ws://10.128.6.214:8554\r\n\r\n";

	while (1)
	{
		responseMsg = "HTTP/1.1 101 Web Socket Protocol Handshake\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\n";

		WaitForSingleObject(hsWebMsgArrived, INFINITE);

		if (msgInQueue.empty())
		{
			continue;
		}

		message = msgInQueue.front();
		msgInQueue.pop();

		string origin = message.msg.substr(message.msg.find("Origin:"));
		origin = origin.substr(7, origin.find("\n") - 6);
		
		origin = "WebSocket-Origin: " + origin;

		responseMsg = responseMsg + origin + srvLocation;

		message.msg = responseMsg;

		msgOutQueue.push(message);

		ReleaseSemaphore(hsWebHandshake, 1, NULL);
	}

	return 0;
}

wsHandler::wsHandler()
{
	CreateThread(NULL, NULL, decodeThreadFunc, NULL, NULL, NULL);
}