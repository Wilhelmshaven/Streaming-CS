/*--Author：李宏杰--*/

#include "middleWare.h"

/*
	加载中间件所需要对接的模块
*/

#include "camCap.h"
#include "cnctHandler.h"
#include "ctrlMsgHandler.h"
#include "imgBuffer.h"
#include "rtpHandler.h"
#include "rtspHandler.h"

#include "logger.h"
#include "monitor.h"

#include "wsHandler.h"

middleWare* middleWare::instance = new middleWare;

namespace mwNS
{
	logger *myLogger = logger::getInstance();
	monitor *myClock = monitor::getInstance();

	//全局事件：结束服务器的事件
	HANDLE heShutdownSrv;

	//图像源出入口
	HANDLE hsRenderOutput;

	//缓存出口
	HANDLE hsImgBufferOutput;

	//
	HANDLE hsCtrlMsgDecoded;

	HANDLE hsRTPEncoded;

	HANDLE hsMsgArrivedRTSP;
	HANDLE hsMsgArrivedCtrl;

	HANDLE hsWebHandshake;
	HANDLE hsWebMsgArrived;
}
using namespace mwNS;

void middleWare::initHandles()
{
	heShutdownSrv = CreateEvent(NULL, TRUE, FALSE, TEXT(syncManager::srvDown));

	hsRenderOutput = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::renderOutput));

	hsImgBufferOutput = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::imgBufferOutput));

	hsCtrlMsgDecoded = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::ctrlMsgDecoded));

	hsRTPEncoded = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::rtpEncoded));

	hsMsgArrivedRTSP = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::msgArrivedRTSP));
	hsMsgArrivedCtrl = CreateSemaphore(NULL, 0, BUF_SIZE, TEXT(syncManager::msgArrivedCtrl));

	hsWebHandshake = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::webHandshake);
	hsWebMsgArrived = CreateSemaphore(NULL, 0, BUF_SIZE, syncManager::webMsgArrived);
}

middleWare* middleWare::getInstance()
{
	return instance;
}

middleWare::middleWare()
{

}

void middleWare::startMiddleWare()
{
	initHandles();

	/*
		创建好各子线程
	*/

	CreateThread(NULL, NULL, mw_Cam_Buf_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Buf_RTP_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_RTP_Cnct_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Cnct_RTSP_Cnct_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Cnct_Ctrl_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Ctrl_Cam_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Cnct_Web_Thread, NULL, NULL, NULL);
	CreateThread(NULL, NULL, mw_Web_Cnct_Thread, NULL, NULL, NULL);

	/*
		调用那些需要手动启动的模块的启动函数
	*/

	cnctHandler *netModule = cnctHandler::getInstance();
	netModule->startServer();

	camCap *camera = camCap::getInstance();
	camera->startCapture();

	myClock->initMonitor(200);

	myLogger->initLogModule();
}

/*
	Renderer/Camera -> Buffer

	从摄像头中取出数据放入缓存
*/
DWORD middleWare::mw_Cam_Buf_Thread(LPVOID lparam)
{
	camCap *camera = camCap::getInstance();

	imgBuffer *buffer = imgBuffer::getInstance();

	SOCKET index;

	imgHead head;

	vector<BYTE> frame;

	while (1)
	{
		//1.等待渲染器/摄像头有图像信号
		WaitForSingleObject(hsRenderOutput, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//开始计时
		//myClock->beginTiming();

		//2.取出图像
		if (!camera->getImage(index, head, frame))
		{
			//103: Can't get image from renderer
			myLogger->logError(103);

			continue;
		}

		/*
			3.塞入缓存
		*/

		//if (!frame.isContinuous())
		//{
		//	//301: [OpenCV]Mat格式的图像不连续，无法转为vector<unsigned char>
		//	errorHandler->handleError(301);

		//	continue;
		//}

		shared_ptr<vector<BYTE>> imgData(new vector<BYTE>);
		(*imgData) = frame;

		buffer->pushBuffer(index, head, imgData);
	}

	return 0;
}

/*
	Buffer -> RTP

	从缓存中取出图像送入RTP模块打包
*/
DWORD middleWare::mw_Buf_RTP_Thread(LPVOID lparam)
{
	imgBuffer *buffer = imgBuffer::getInstance();

	rtpHandler *rtp = rtpHandler::getInstance();

	SOCKET index;

	imgHead head;
	shared_ptr<vector<BYTE>> imgData;

	while (1)
	{
		//1.等待缓存有图像的信号
		WaitForSingleObject(hsImgBufferOutput, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//2.从缓存中取出图像
		if (!buffer->popBuffer(index, head, imgData))
		{
			//106:Can't get image from Image Buffer
			myLogger->logError(106);

			continue;
		}

		/*
			3.把图像送入RTP模块
		*/

		if (!rtp->pack(index, head, imgData))
		{
			//401. Session is down
			myLogger->logError(401);

			continue;
		}
	}

	return 0;
}

/*
	RTP -> Network

	从RTP模块中取出数据包送入网络模块发送
*/
DWORD middleWare::mw_RTP_Cnct_Thread(LPVOID lparam)
{
	cnctHandler *network = cnctHandler::getInstance();

	rtpHandler *rtp = rtpHandler::getInstance();

	SOCKET index;

	shared_ptr<string> packet(new string);

	while (1)
	{
		//1.等待RTP打包好的信号
		WaitForSingleObject(hsRTPEncoded, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//2.取出打包好的数据包
		if (!rtp->getPacket(index, packet))
		{
			//104:Can't get packed packet from RTP module
			myLogger->logError(104);

			continue;
		}

		//3.将数据包送入网络模块发送
		network->sendMessage((*packet), index);

		//结束计时
		//myClock->endTiming();	
	}

	return 0;
}

/*
	Network -> RTSP -> Network

	直接完成一个完整的RTSP解析与回复（私以为这里可以不异步）
*/
DWORD middleWare::mw_Cnct_RTSP_Cnct_Thread(LPVOID lparam)
{
	cnctHandler *network = cnctHandler::getInstance();

	rtspHandler *rtsp = rtspHandler::getInstance();

	SOCKET index;

	string msg;

	while (1)
	{
		//1.等待服务器收到RTSP消息
		WaitForSingleObject(hsMsgArrivedRTSP, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//2.从网络模块取出消息
		if (!network->getRTSPMsg(msg, index))
		{
			//201:Can't get RTSP message from network module
			myLogger->logError(201);

			continue;
		}

		//3.解码并获得回复
		msg = rtsp->msgCodec(index, msg);

		//4.送入网络模块发送
		network->sendMessage(msg, index);
	}

	return 0;
}

/*
	Network -> Ctrl

	将收到的控制信令消息进行解码
*/
DWORD middleWare::mw_Cnct_Ctrl_Thread(LPVOID lparam)
{
	cnctHandler *network = cnctHandler::getInstance();

	ctrlMsgHandler *ctrl = ctrlMsgHandler::getInstance();

	SOCKET index;

	string msg;

	while (1)
	{
		//1.等待服务器收到控制信息
		WaitForSingleObject(hsMsgArrivedCtrl, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//2.从网络模块取出控制信息
		if (!network->getCtrlMsg(msg, index))
		{
			//101:Can't get control message from network module
			myLogger->logError(101);

			continue;
		}

		//3.解码信令
		ctrl->decodeMsg(index, msg);
	}

	return 0;
}

/*
	Ctrl -> Camera/Renderer

	将解码好的控制指令送给渲染器/摄像头
*/
DWORD middleWare::mw_Ctrl_Cam_Thread(LPVOID lparam)
{
	ctrlMsgHandler *ctrl = ctrlMsgHandler::getInstance();

	camCap *camera = camCap::getInstance();

	SOCKET index;

	unsigned char key;

	unsigned int session;

	while (1)
	{
		//1.等待解码好的信号
		WaitForSingleObject(hsCtrlMsgDecoded, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//2.取出解码好的消息
		if (!ctrl->getDecodedMsg(index, session, key))
		{
			//102:Can't get decoded control msg from decodoer
			myLogger->logError(102);

			continue;
		}

		//3.将解码好的消息送入渲染器/摄像头
		camera->render(index, key);
	}

	return 0;
}

/*
	测试用代码：WebSocket，服务于Web端
	连接模块到Web模块
*/
DWORD middleWare::mw_Cnct_Web_Thread(LPVOID lparam)
{
	cnctHandler *network = cnctHandler::getInstance();

	wsHandler *websocket = wsHandler::getInstance();

	SOCKET index;

	string msg;

	while (1)
	{
		WaitForSingleObject(hsWebMsgArrived, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		if (!network->getHTTPMsg(msg, index))
		{
			//502,Can't get message from network module.
			myLogger->logError(502);
			
			continue;
		}

		websocket->decodeMsg(index, msg);
	}

	return 0;
}

/*
	测试用代码：WebSocket，服务于Web端
	Web模块到连接模块
*/
DWORD middleWare::mw_Web_Cnct_Thread(LPVOID lparam)
{
	cnctHandler *network = cnctHandler::getInstance();

	wsHandler *websocket = wsHandler::getInstance();

	SOCKET index;

	string msg;

	while (1)
	{
		WaitForSingleObject(hsWebHandshake, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		if (!websocket->getResponse(index, msg))
		{
			//501,Can't get message from websocket module.
			myLogger->logError(501);

			continue;
		}

		network->sendMessage(msg, index);
	}

	return 0;
}
