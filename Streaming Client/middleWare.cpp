/*--Author：李宏杰--*/

#include "middleWare.h"

#include "xPlayer.h"

#include "ctrlMsgHandler.h"
#include "imageBuffer.h"
#include "rtpHandler.h"
#include "rtspHandler.h"

#include "cnctHandler.h"

#include "errHandler.h"

//计时器模块：用于获取延迟数据
#include "monitor.h"

#include "logger.h"

namespace mwNS
{
	errHandler *errorHandler = errHandler::getInstance();

	monitor *clock = monitor::getInstance();

	logger *myLogger = logger::getInstance();

	xPlayer *player = xPlayer::getInstance();

	//关闭服务器事件
	HANDLE heCloseClientAll;

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
}
using namespace mwNS;

middleWare* middleWare::instance = new middleWare;

middleWare * middleWare::getInstance()
{
	return instance;
}

void middleWare::startMiddleWare()
{
	cnctHandler *network = cnctHandler::getInstance();

	rtspHandler *rtsp = rtspHandler::getInstance();

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

	myLogger->initLogModule();

	//连接服务器
	if (network->connectServer() == 0)
	{
		CreateThread(NULL, NULL, mw_RTSP_Net_RTSP_Thread, NULL, NULL, NULL);

		//player->setFrameRate(20);
		//glPlayer->不需要设置

		player->startPlayer();

		//初始化计时器
		mwNS::clock->initMonitor();
	}
	else
	{
		//100,Can't connect to server.
		myLogger->logError(100);

		SetEvent(heCloseClientAll);
	}
}

/*
	开始清理工作
	首先把结束事件打开，再依次打开各信号量使其能够检查结束事件
*/
void middleWare::shutdownAll()
{
	SetEvent(heCloseClientAll);

	ReleaseSemaphore(hsCvPlayerOutput, 1, NULL);
	ReleaseSemaphore(hsCtrlOutput, 1, NULL);
	ReleaseSemaphore(hsRecvRTSPMsg, 1, NULL);
	ReleaseSemaphore(hsRecvRTPMsg, 1, NULL);
	ReleaseSemaphore(hsRTPUnpacked, 1, NULL);
	ReleaseSemaphore(hsBufOutput, 1, NULL);

	mwNS::clock->shutdown();

	myLogger->shutdownModule();
}

void middleWare::initHandles()
{
	heCloseClientAll = CreateEvent(NULL, TRUE, FALSE, syncManager::clientClose);

	hsCvPlayerOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::playerOutput);

	hsCtrlOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::ctrlMsgOutput);

	hsRecvRTSPMsg = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::cnctRTSPOutput);

	hsRecvRTPMsg = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::cnctRTPOutput);

	hsRTPUnpacked = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::rtpOutput);

	hsBufOutput = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::bufferOutput);
}

DWORD middleWare::mw_Player_Ctrl_Thread(LPVOID lparam)
{
	ctrlMsgHandler *ctrl = ctrlMsgHandler::getInstance();

	rtspHandler *rtsp = rtspHandler::getInstance();

	unsigned char key;

	unsigned long session;

	while (1)
	{
		WaitForSingleObject(hsCvPlayerOutput, INFINITE);

		if (WaitForSingleObject(heCloseClientAll, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		mwNS::clock->beginTiming();

		if (!player->getCtrlKey(key))
		{
			//101,Can't get control key from player.
			myLogger->logError(101);

			continue;
		}

		//这里会出错：如果没有会话号 
		session = stoul(rtsp->getSession(), nullptr, 16);

		ctrl->keyboardMsgEncode(key, session);
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

		if (WaitForSingleObject(heCloseClientAll, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		if (!ctrl->getEncodedMsg(encodedMsg))
		{
			//102,Can't get encoded control message from encoder.
			myLogger->logError(102);

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

	shared_ptr<vector<BYTE>> ptr;

	while (1)
	{
		WaitForSingleObject(hsRecvRTPMsg, INFINITE);

		if (WaitForSingleObject(heCloseClientAll, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//启动计时器
		//clock->beginTiming();

		if (!network->getRTPMessage(ptr))
		{
			//104,Can't get RTP message from network handler.
			myLogger->logError(104);

			continue;
		}

		rtp->unpackRTP(ptr);
	}

	return 0;
}

DWORD middleWare::mw_RTP_Buf_Thread(LPVOID lparam)
{
	rtpHandler *rtp = rtpHandler::getInstance();

	imgBuffer *buffer = imgBuffer::getInstance();

	imgHead head;
	vector<unsigned char> imgData;

	shared_ptr<vector<BYTE>> ptr;

	while (1)
	{
		WaitForSingleObject(hsRTPUnpacked, INFINITE);

		if (WaitForSingleObject(heCloseClientAll, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		if (!rtp->getMedia(head, ptr))
		{
			//105,Can't get media from RTP module.
			myLogger->logError(105);

			continue;
		}

		buffer->pushBuffer(head, ptr);

		mwNS::clock->endTiming();
	}

	return 0;
}

DWORD middleWare::mw_Buf_Player_Thread(LPVOID lparam)
{
	imgBuffer *buffer = imgBuffer::getInstance();

	imgHead head;
	vector<unsigned char> imgData;

	shared_ptr<vector<BYTE>> ptr;

	while (1)
	{
		WaitForSingleObject(hsBufOutput, INFINITE);

		if (WaitForSingleObject(heCloseClientAll, 0) == WAIT_OBJECT_0)
		{
			break;
 		}

		if (!buffer->popBuffer(head, ptr))
		{
			//106,Can't get media from buffer.
			myLogger->logError(106);

			continue;
		}

		////测试代码#3
		//double timestamp3;
		//clock->getTimeStamp(timestamp3);
		//myLogger->insertTimestamp(3, timestamp3);

		player->insertImage(head, ptr);

		////测试代码#4
		//double timestamp4;
		//clock->getTimeStamp(timestamp4);
		//myLogger->insertTimestamp(4, timestamp4);

		//结束计时器
		//clock->endTiming();
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

//小封装
void middleWare::sendRTSPMsg(int method)
{
	cnctHandler *network = cnctHandler::getInstance();

	rtspHandler *rtsp = rtspHandler::getInstance();

	string msg;

	msg = rtsp->encodeMsg(method);

	network->sendMessage(msg);
}

//小封装
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
			myLogger->logError(103);

			break;
		}

		errCode = rtsp->decodeMsg(msg);

	} while (0);
}

middleWare::middleWare()
{
	initHandles();
}