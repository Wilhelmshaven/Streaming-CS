/*--Author：李宏杰--*/

#include "myServer.h"

/*
	加载中间件所需要对接的模块
*/

#include "camCap.h"
#include "cnctHandler.h"
#include "ctrlMsgHandler.h"
#include "imgBuffer.h"
#include "rtpHandler.h"
#include "rtspHandler.h"

#include "errHandler.h"

/*
	专门输出前置提示信息
*/
myMessage::myMessage()
{
	cout << "================================================" << endl;
	cout << "=                                              =" << endl;
	cout << "=     交互式流媒体序列帧传输平台，版本v1.0     =" << endl;
	cout << "=            143020085211001 李宏杰            =" << endl;
	cout << "=                                              =" << endl;
	cout << "================================================" << endl;
	cout << endl << "--初始化中..." << endl;
	cout << "--监听连接..." << endl;
};
const myMessage myMsg;

/*
	信号量/互斥量/事件
*/
namespace myHandlerName
{
	//全局事件：结束服务器的事件
	HANDLE heShutdownSrv;

	//图像源出入口
	HANDLE hsRenderInput;
	HANDLE hsRenderOutput;

	//缓存出入口
	HANDLE hsImgBufferInput;
	HANDLE hsImgBufferOutput;

	//流媒体信令多个出口
	HANDLE hsRTSPPlay;
	HANDLE hsRTSPPause;
	HANDLE hsRTSPStop;

	//
	HANDLE hsCtrlMsgDecoded;
	HANDLE hsCtrlMsgEncoded;

	HANDLE hsRTPEncoded;

	HANDLE hsMsgArrivedRTSP;
	HANDLE hsMsgArrivedCtrl;
}

using namespace myNS;

/*
	初始化服务器
*/
void initServer()
{

	heShutdownSrv = CreateEvent(NULL, TRUE, FALSE, NULL);

	hsRenderInput = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));
	hsRenderOutput = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));

	//hsImgBufferInput= CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::imgBufferInput));
	hsImgBufferOutput = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::imgBufferOutput));

	hsCtrlMsgDecoded = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::ctrlMsgDecoded));
	hsCtrlMsgEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::ctrlMsgEncoded));

	hsRTPEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::rtpEncoded));

	hsMsgArrivedRTSP = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::msgArrivedRTSP));
	hsMsgArrivedCtrl = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::msgArrivedCtrl));
}

//================================= MAIN =================================//
int main()
{
	initServer();

	cnctHandler *netModule = cnctHandler::getInstance();

	netModule->startServer();

	middleWare *midWare = middleWare::getInstance();

	midWare->startMiddleWare();

	camCap *camera = camCap::getInstance();

	camera->startCapture();

	WaitForSingleObject(heShutdownSrv, INFINITE);

	/*
		剩余清理工作
	*/

	//camera->stopCapture();

	return 0;
}

//================================= 中间件代码部分 =================================//

middleWare* middleWare::instance = new middleWare;

middleWare * middleWare::getInstance()
{
	return instance;
}

void middleWare::startMiddleWare()
{
	CreateThread(NULL, NULL, mwCtrlMsgThread, NULL, NULL, NULL);

	CreateThread(NULL, NULL, mwRTSPMsgThread, NULL, NULL, NULL);
}

void middleWare::initHandles()
{
	heShutdownSrv = CreateEvent(NULL, TRUE, FALSE, NULL);

	//hsRenderInput = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderInput));
	hsRenderOutput = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));

	//hsImgBufferInput= CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::imgBufferInput));
	hsImgBufferOutput = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::imgBufferOutput));

	hsCtrlMsgDecoded = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::ctrlMsgDecoded));
	hsCtrlMsgEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::ctrlMsgEncoded));

	hsRTPEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::rtpEncoded));

	hsMsgArrivedRTSP = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::msgArrivedRTSP));
	hsMsgArrivedCtrl = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::msgArrivedCtrl));
}

DWORD middleWare::mwCtrlMsgThread(LPVOID lparam)
{
	cnctHandler *netModule = cnctHandler::getInstance();
	ctrlMsgHandler *ctrlMsgModule = ctrlMsgHandler::getInstance();
	camCap *renderer = camCap::getInstance();
	imgBuffer *imgBufModule = imgBuffer::getInstance();
	rtpHandler *rtpModule = rtpHandler::getInstance();

	errHandler *errorHandler = errHandler::getInstance();

	string ctrlMsg;
	SOCKET clientSocket;

	unsigned int session;
	unsigned char key;

	imgHead iHead;
	vector<unsigned char> imgData;

	string rtpMsg;

	string imgHeadMsg;

	while (1)
	{
		WaitForSingleObject(hsMsgArrivedCtrl, INFINITE);

		//1.从网络模块拿信令以及SOCKET
		if (!netModule->getCtrlMsg(ctrlMsg, clientSocket))
		{
			errorHandler->handleError(101);

			continue;
		}

		//2.丢给控制信令处理模块，并获取会话号
		ctrlMsgModule->decodeMsg(ctrlMsg);

		//3.等解码好了，拿回来
		WaitForSingleObject(hsCtrlMsgDecoded, INFINITE);

		if (!ctrlMsgModule->getDecodedMsg(session, key))
		{
			errorHandler->handleError(102);

			continue;
		}

		//4.然后塞给渲染器
		renderer->render(key);

		//这里漏了一大截！！

		//5.然后等图像好了，取出来。注意，从图像缓存中取-->错啦！！！
		WaitForSingleObject(hsRenderOutput, INFINITE);

		if (!imgBufModule->popBuffer(iHead, imgData))
		{
			errorHandler->handleError(103);

			continue;
		}

		//6.把图像塞给RTP打包模块，并且推入SOCKET以构建序列号
		rtpModule->pack(clientSocket, imgData);

		WaitForSingleObject(hsRTPEncoded, INFINITE);

		if (!rtpModule->getPacket(rtpMsg))
		{
			errorHandler->handleError(104);

			continue;
		}

		//7.编码图像头
		ctrlMsgModule->encodeMsg(iHead, imgData.size(), session);

		WaitForSingleObject(hsCtrlMsgEncoded, INFINITE);

		if (!ctrlMsgModule->getEncodedMsg(imgHeadMsg))
		{
			errorHandler->handleError(105);

			continue;
		}

		//8.把RTP包交给网络模块发送。先发送头再发送数据
		netModule->sendMessage(imgHeadMsg, clientSocket);
		netModule->sendMessage(rtpMsg, clientSocket);
	}

	return 0;
}

/*
	专门处理流媒体信令的线程
	对接网络模块与流媒体信令处理模块
*/
DWORD middleWare::mwRTSPMsgThread(LPVOID lparam)
{

	cnctHandler *netModule = cnctHandler::getInstance();
	rtspHandler *rtspModule = rtspHandler::getInstance();

	errHandler *errorHandler = errHandler::getInstance();

	string rtspMsg;
	SOCKET clientSocket;

	string respondMsg;

	while (1)
	{
		WaitForSingleObject(hsMsgArrivedRTSP, INFINITE);

		//1.从网络模块获取信令
		if (!netModule->getRTSPMsg(rtspMsg, clientSocket))
		{
			errorHandler->handleError(201);

			continue;
		}

		//2.调用流媒体信令处理模块解码并获得答令
		respondMsg = rtspModule->msgCodec(clientSocket, rtspMsg);

		//3.调用网络模块发送答令
		netModule->sendMessage(respondMsg, clientSocket);
	}

	return 0;
}

middleWare::middleWare()
{

}