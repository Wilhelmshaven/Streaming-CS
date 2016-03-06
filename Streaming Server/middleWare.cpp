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
#include "ctrlMsgHandler.h"

//加载摄像头（模拟渲染器）模块
#include "camCap.h"

//加载图像缓存
#include "imageQueue.h"

//加载RTP打包模块
#include "rtpHandler.h"

mwMsg* mwMsg::instance = new mwMsg;

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
	cnctHandler *netModule = cnctHandler::getInstance();

	ctrlMsgHandler *ctrlMsgModule = ctrlMsgHandler::getInstance();

	camCap *renderer = camCap::getInstance();

	imgBuffer *imgBuf = imgBuffer::getInstance();

	rtpHandler *rtpModule = rtpHandler::getInstance();

	string ctrlMsg;

	char key;

	vector<int> img;

	imgHead imageHead;

	string rtpPacket;

	string imageHeadMsg;

	int session;

	SOCKET clientSocket;

	while (1)
	{
		WaitForSingleObject(hsCtrlMsgArrived, INFINITE);

		if (WaitForSingleObject(hSrvShutdown, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//1.从网络模块拿信令以及SOCKET！
		ctrlMsg = netModule->getCtrlMsg();

		clientSocket = netModule->getSocket();

		//2.丢给控制信令处理模块，并获取会话号
		session = ctrlMsgModule->decodeMsg(ctrlMsg);

		//3.等解码好了，拿回来
		key = 0;

		WaitForSingleObject(hsCtrlMsg, INFINITE);

		key = ctrlMsgModule->getCtrlKey();

		//4.然后塞给渲染器
		renderer->render(key);

		//5.然后等图像好了，取出来。注意，从图像缓存中取
		WaitForSingleObject(hsImageReady, INFINITE);

		imgBuf->popBuffer(imageHead, img);

		//6.把图像塞给RTP打包模块，并且推入SOCKET以构建序列号
		rtpPacket = rtpModule->pack(clientSocket, img);

		//7.编码图像头
		imageHeadMsg = ctrlMsgModule->encodeMsg(imageHead, img.size(), session);

		//8.把RTP包交给网络模块发送。先发送头再发送数据
		netModule->sendMessage(clientSocket, imageHeadMsg);

		netModule->sendMessage(clientSocket, rtpPacket);
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
		rtspMsg = rtspModule->msgCodec(clientSocket, rtspMsg);

		//4.调用网络模块发送答令
		bytesSent = netModule->sendMessage(clientSocket, rtspMsg);
	}

	return 0;
}

mwMsg::mwMsg()
{

}