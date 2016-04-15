/*--Author：李宏杰--*/

#include "middleWare.h"

#include "cvPlayer.h"
#include "ctrlMsgHandler.h"
#include "imageBuffer.h"
#include "rtpHandler.h"
#include "rtspHandler.h"

#include "cnctHandler.h"

#include "errHandler.h"

errHandler *errorHandler = errHandler::getInstance();

//关闭服务器事件
HANDLE heCloseClient;

//播放器出口
HANDLE hsCvPlayerOutput;

//控制信令编码器出口
HANDLE hsCtrlOutput;

//标记网络模块收到RTSP信息
HANDLE hsRecvRTSPMsg;

//标记网络模块收到RTP信息
HANDLE hsRecvRTPMsg;

//标记RTP解包完成
HANDLE hsRTPUnpacked;

//标记图像缓存中有图像
HANDLE hsBufOutput;

middleWare* middleWare::instance = new middleWare;

middleWare * middleWare::getInstance()
{
	return instance;
}

void middleWare::startMiddleWare()
{
	cnctHandler *network = cnctHandler::getInstance();

	rtspHandler *rtsp = rtspHandler::getInstance();

	cvPlayer *player = cvPlayer::getInstance();

	/*
		创建好各子线程
	*/

	CreateThread(NULL, NULL, mw_Player_Ctrl_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Ctrl_Net_Thread, NULL, NULL, NULL);	
	CreateThread(NULL, NULL, mw_Net_RTP_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_RTP_Buf_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Buf_Player_Thread, NULL, NULL, NULL);

	/*
		调用那些需要手动启动的模块的启动函数
	*/

	rtsp->setHandler(network->getDisplayAddr());

	//连接服务器
	if (network->connectServer() == 0)
	{
		CreateThread(NULL, NULL, mw_RTSP_Net_RTSP_Thread, NULL, NULL, NULL);

		player->setFrameRate(20);

		player->play();
	}
	else
	{
		//100,Can't connect to server.
		errorHandler->handleError(100);

		SetEvent(heCloseClient);
	}
}

void middleWare::initHandles()
{
	heCloseClient = CreateEvent(NULL, TRUE, FALSE, syncManager::clientClose);

	hsCvPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);

	hsCtrlOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::ctrlMsgOutput);

	hsRecvRTSPMsg = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::cnctRTSPOutput);

	hsRecvRTPMsg = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::cnctRTPOutput);

	hsRTPUnpacked = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::rtpOutput);

	hsBufOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::bufferOutput);
}

DWORD middleWare::mw_Player_Ctrl_Thread(LPVOID lparam)
{
	cvPlayer *player = cvPlayer::getInstance();

	ctrlMsgHandler *ctrl = ctrlMsgHandler::getInstance();

	char key;

	while (1)
	{
		WaitForSingleObject(hsCvPlayerOutput, INFINITE);

		if (WaitForSingleObject(heCloseClient, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		if (!player->getCtrlKey(key))
		{
			//101,Can't get control key from player.
			errorHandler->handleError(101);

			continue;
		}

		ctrl->keyboardMsgEncode(key);
	}

	return 0;
}

DWORD middleWare::mw_Ctrl_Net_Thread(LPVOID lparam)
{
	ctrlMsgHandler *ctrl = ctrlMsgHandler::getInstance();

	cnctHandler *network = cnctHandler::getInstance();

	string encodedMsg;

	while (1)
	{
		WaitForSingleObject(hsCtrlOutput, INFINITE);

		if (WaitForSingleObject(heCloseClient, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		if (!ctrl->getEncodedMsg(encodedMsg))
		{
			//102,Can't get encoded control message from encoder.
			errorHandler->handleError(102);

			continue;
		}

		network->sendMessage(encodedMsg);
	}

	return 0;
}

/*
	RTSP交互线程
	
	完成整个RTSP交互的流水线（其实比较属于一次性工程的说）
*/
DWORD middleWare::mw_RTSP_Net_RTSP_Thread(LPVOID lparam)
{
	cnctHandler *network = cnctHandler::getInstance();

	rtspHandler *rtsp = rtspHandler::getInstance();

	string msg;

	int errCode;

	do
	{
		sendRTSPMsg(OPTIONS);

		recvRTSPMsg(errCode);

		if (errCode != 200)break;

		sendRTSPMsg(DESCRIBE);

		recvRTSPMsg(errCode);

		if (errCode != 200)break;

		sendRTSPMsg(SETUP);

		recvRTSPMsg(errCode);

		if (errCode != 200)break;

		sendRTSPMsg(PLAY);

		recvRTSPMsg(errCode);

		if (errCode != 200)break;

	} while (0);

	return 0;
}

DWORD middleWare::mw_Net_RTP_Thread(LPVOID lparam)
{
	cnctHandler *network = cnctHandler::getInstance();

	rtpHandler *rtp = rtpHandler::getInstance();

	string msg;

	while (1)
	{
		WaitForSingleObject(hsRecvRTPMsg, INFINITE);

		if (WaitForSingleObject(heCloseClient, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		if (!network->getRTPMessage(msg))
		{
			//104,Can't get RTP message from network handler.
			errorHandler->handleError(104);

			continue;
		}

		rtp->unpackRTP(msg);
	}

	return 0;
}

DWORD middleWare::mw_RTP_Buf_Thread(LPVOID lparam)
{
	rtpHandler *rtp = rtpHandler::getInstance();

	imgBuffer *buffer = imgBuffer::getInstance();

	imgHead head;
	vector<unsigned char> imgData;

	while (1)
	{
		WaitForSingleObject(hsRTPUnpacked, INFINITE);

		if (WaitForSingleObject(heCloseClient, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		if (!rtp->getMedia(head, imgData))
		{
			//105,Can't get media from RTP module.
			errorHandler->handleError(105);

			continue;
		}

		buffer->pushBuffer(head, imgData);
	}

	return 0;
}

DWORD middleWare::mw_Buf_Player_Thread(LPVOID lparam)
{
	imgBuffer *buffer = imgBuffer::getInstance();

	cvPlayer *player = cvPlayer::getInstance();

	imgHead head;
	vector<unsigned char> imgData;

	while (1)
	{
		WaitForSingleObject(hsBufOutput, INFINITE);

		if (WaitForSingleObject(heCloseClient, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		if (!buffer->popBuffer(head, imgData))
		{
			//106,Can't get media from buffer.
			errorHandler->handleError(106);

			continue;
		}

		player->insertImage(head, imgData);
	}

	return 0;
}

/*
	心跳线程
	但现在是TCP传输的话，就不用了
*/
//DWORD middleWare::mwHeartBeat(LPVOID lparam)
//{
//	cnctHandler *networkModule = cnctHandler::getInstance();
//
//	rtspHandler *rtspModule = rtspHandler::getInstance();
//
//	string msg;
//
//	WaitForSingleObject(heStartHeartBeat, INFINITE);
//
//	while (1)
//	{
//		if (WaitForSingleObject(heStartHeartBeat, 0) != WAIT_OBJECT_0)
//		{
//			break;
//		}
//
//		/*
//			每30秒发送一次心跳
//		*/
//
//		Sleep(30000);
//
//		msg = rtspModule->encodeMsg(GET_PARAMETER);
//
//		networkModule->sendMessage(msg);
//	}
//
//	return 0;
//}

void middleWare::sendRTSPMsg(int method)
{
	cnctHandler *network = cnctHandler::getInstance();

	rtspHandler *rtsp = rtspHandler::getInstance();

	string msg;

	msg = rtsp->encodeMsg(method);

	network->sendMessage(msg);
}

void middleWare::recvRTSPMsg(int & errCode)
{
	cnctHandler *network = cnctHandler::getInstance();

	rtspHandler *rtsp = rtspHandler::getInstance();

	string msg;

	WaitForSingleObject(hsRecvRTSPMsg, INFINITE);

	do
	{
		if (!network->getRTSPMessage(msg))
		{
			errCode = 103;

			//103,Can't get RTSP message from network handler.
			errorHandler->handleError(errCode);

			break;
		}

		errCode = rtsp->decodeMsg(msg);

	} while (0);
}

middleWare::middleWare()
{
	initHandles();
}