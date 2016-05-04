/*--Author������--*/

#include "middleWare.h"

/*
	�����м������Ҫ�Խӵ�ģ��
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

	//ȫ���¼����������������¼�
	HANDLE heShutdownSrv;

	//ͼ��Դ�����
	HANDLE hsRenderOutput;

	//�������
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
		�����ø����߳�
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
		������Щ��Ҫ�ֶ�������ģ�����������
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

	������ͷ��ȡ�����ݷ��뻺��
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
		//1.�ȴ���Ⱦ��/����ͷ��ͼ���ź�
		WaitForSingleObject(hsRenderOutput, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//��ʼ��ʱ
		//myClock->beginTiming();

		//2.ȡ��ͼ��
		if (!camera->getImage(index, head, frame))
		{
			//103: Can't get image from renderer
			myLogger->logError(103);

			continue;
		}

		/*
			3.���뻺��
		*/

		//if (!frame.isContinuous())
		//{
		//	//301: [OpenCV]Mat��ʽ��ͼ���������޷�תΪvector<unsigned char>
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

	�ӻ�����ȡ��ͼ������RTPģ����
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
		//1.�ȴ�������ͼ����ź�
		WaitForSingleObject(hsImgBufferOutput, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//2.�ӻ�����ȡ��ͼ��
		if (!buffer->popBuffer(index, head, imgData))
		{
			//106:Can't get image from Image Buffer
			myLogger->logError(106);

			continue;
		}

		/*
			3.��ͼ������RTPģ��
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

	��RTPģ����ȡ�����ݰ���������ģ�鷢��
*/
DWORD middleWare::mw_RTP_Cnct_Thread(LPVOID lparam)
{
	cnctHandler *network = cnctHandler::getInstance();

	rtpHandler *rtp = rtpHandler::getInstance();

	SOCKET index;

	shared_ptr<string> packet(new string);

	while (1)
	{
		//1.�ȴ�RTP����õ��ź�
		WaitForSingleObject(hsRTPEncoded, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//2.ȡ������õ����ݰ�
		if (!rtp->getPacket(index, packet))
		{
			//104:Can't get packed packet from RTP module
			myLogger->logError(104);

			continue;
		}

		//3.�����ݰ���������ģ�鷢��
		network->sendMessage((*packet), index);

		//������ʱ
		//myClock->endTiming();	
	}

	return 0;
}

/*
	Network -> RTSP -> Network

	ֱ�����һ��������RTSP������ظ���˽��Ϊ������Բ��첽��
*/
DWORD middleWare::mw_Cnct_RTSP_Cnct_Thread(LPVOID lparam)
{
	cnctHandler *network = cnctHandler::getInstance();

	rtspHandler *rtsp = rtspHandler::getInstance();

	SOCKET index;

	string msg;

	while (1)
	{
		//1.�ȴ��������յ�RTSP��Ϣ
		WaitForSingleObject(hsMsgArrivedRTSP, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//2.������ģ��ȡ����Ϣ
		if (!network->getRTSPMsg(msg, index))
		{
			//201:Can't get RTSP message from network module
			myLogger->logError(201);

			continue;
		}

		//3.���벢��ûظ�
		msg = rtsp->msgCodec(index, msg);

		//4.��������ģ�鷢��
		network->sendMessage(msg, index);
	}

	return 0;
}

/*
	Network -> Ctrl

	���յ��Ŀ���������Ϣ���н���
*/
DWORD middleWare::mw_Cnct_Ctrl_Thread(LPVOID lparam)
{
	cnctHandler *network = cnctHandler::getInstance();

	ctrlMsgHandler *ctrl = ctrlMsgHandler::getInstance();

	SOCKET index;

	string msg;

	while (1)
	{
		//1.�ȴ��������յ�������Ϣ
		WaitForSingleObject(hsMsgArrivedCtrl, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//2.������ģ��ȡ��������Ϣ
		if (!network->getCtrlMsg(msg, index))
		{
			//101:Can't get control message from network module
			myLogger->logError(101);

			continue;
		}

		//3.��������
		ctrl->decodeMsg(index, msg);
	}

	return 0;
}

/*
	Ctrl -> Camera/Renderer

	������õĿ���ָ���͸���Ⱦ��/����ͷ
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
		//1.�ȴ�����õ��ź�
		WaitForSingleObject(hsCtrlMsgDecoded, INFINITE);

		if (WaitForSingleObject(heShutdownSrv, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//2.ȡ������õ���Ϣ
		if (!ctrl->getDecodedMsg(index, session, key))
		{
			//102:Can't get decoded control msg from decodoer
			myLogger->logError(102);

			continue;
		}

		//3.������õ���Ϣ������Ⱦ��/����ͷ
		camera->render(index, key);
	}

	return 0;
}

/*
	�����ô��룺WebSocket��������Web��
	����ģ�鵽Webģ��
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
	�����ô��룺WebSocket��������Web��
	Webģ�鵽����ģ��
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
