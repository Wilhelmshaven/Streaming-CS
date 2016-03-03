/*--Author：李宏杰--*/

#include "middleWare.h"

/*
	加载该中间件所需要对接的模块
*/

//加载网络处理模块
#include "cnctHandler.h"

//加载流媒体信令处理模块
#include "rtspHandler.h"

//加载控制信令处理模块
#include "crtlMsgHandler.h"

mwMsg * mwMsg::getInstance()
{
	return instance;
}

void mwMsg::startMiddleWare()
{
	CreateThread(NULL, NULL, mwCtrlMsgThread, NULL, NULL, NULL);

	CreateThread(NULL, NULL, mwRTSPMsgThread, NULL, NULL, NULL);
}

DWORD mwMsg::mwCtrlMsgThread(LPVOID lparam)
{

	while (1)
	{
		WaitForSingleObject(hsCtrlMsgArrived, INFINITE);

		if (WaitForSingleObject(hSrvShutdown, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//TODO：处理控制信令
	}

	return 0;
}

/*
	专门处理流媒体信令的线程
	对接网络模块与流媒体信令处理模块
*/
DWORD mwMsg::mwRTSPMsgThread(LPVOID lparam)
{

	cnctHandler *netModule = cnctHandler::getInstance();

	rtspHandler *rtspModule = rtspHandler::getInstance();

	SOCKET clientSocket;
	string rtspMsg;

	int bytesSent;

	while (1)
	{
		WaitForSingleObject(hsRTSPMsgArrived, INFINITE);

		if (WaitForSingleObject(hSrvShutdown, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//1.从网络模块获取信令
		rtspMsg = netModule->getRTSPMsg();

		//2.从网络模块获取Socket
		clientSocket = netModule->getSocket();

		//3.调用流媒体信令处理模块解码并获得答令
		rtspMsg = rtspModule->msgCodec(rtspMsg);

		//4.调用网络模块发送答令
		bytesSent = netModule->sendMessage(clientSocket, rtspMsg);
	}

	return 0;
}
