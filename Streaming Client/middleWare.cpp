/*--Author������--*/

#include "middleWare.h"

//���ز�����ģ��
#include "cvPlayer.h"

//���ؿ��������ģ��
#include "msgHandler.h"

//�������紦��ģ��
#include "cnctHandler.h"

//����ý�建��
#include "imageQueue.h"

//����RTP���ݰ�����ģ��
#include "rtpHandler.h"

//���ؼ��ģ��
#include "monitor.h"

//����RTSP�����ģ��
#include "rtspHandler.h"

mwPlayCtrl* mwPlayCtrl::instance = new mwPlayCtrl;

mwPlayCtrl * mwPlayCtrl::getInstance()
{
	return instance;
}

void mwPlayCtrl::startMiddleWare()
{
	CreateThread(NULL, NULL, mwCtrlMsgThread, NULL, NULL, NULL);

	CreateThread(NULL, NULL, mwMediaThread, NULL, NULL, NULL);

	CreateThread(NULL, NULL, mwHeartBeat, NULL, NULL, NULL);

	/*
		����һ����ý��
	*/

	cnctHandler *networkModule = cnctHandler::getInstance();

	rtspHandler *rtspModule = rtspHandler::getInstance();

	rtspModule->setHandler(networkModule->getDisplayAddr());
}

DWORD mwPlayCtrl::mwCtrlMsgThread(LPVOID lparam)
{
	cvPlayer *playerModule = cvPlayer::getInstance();

	msgHandler *ctrlMsgModule = msgHandler::getInstance();

	cnctHandler *networkModule = cnctHandler::getInstance();

	monitor *timingModule = monitor::getInstance();

	while (1)
	{
		//1.ȡ�߿��ư���������ʼ��ʱ
		WaitForSingleObject(hsNewCtrlKey, INFINITE);

		timingModule->beginTiming();

		char key = 0;
		key = playerModule->getCtrlKey();

		//2.�ѿ��ư����������������ģ�����֮����ȡ�ر�����
		string msg;
		msg = ctrlMsgModule->keyboardMsgEncode(key);

		//3.�ѱ���õ����������ģ�鷢��֮
		networkModule->sendMessage(msg);
	}

	return 0;
}

DWORD mwPlayCtrl::mwMediaThread(LPVOID lparam)
{
	imgBuffer *imgBuf = imgBuffer::getInstance();

	rtpHandler *rtpModule = rtpHandler::getInstance();

	monitor *timingModule = monitor::getInstance();

	cnctHandler *networkModule = cnctHandler::getInstance();

	rtspHandler *rtspModule = rtspHandler::getInstance();

	image img;

	while (1)
	{
		/*
			1. ������ģ���������ݣ���ȡ��
		*/
		string incomingMsg;

		WaitForSingleObject(hsNewRecvMsg, INFINITE);

		networkModule->getRecvMessage(incomingMsg);

		/*
			2. ���ֽ��յ��������������ý��
		*/

		if (incomingMsg[0] == '$')
		{
			//ý������

			//3.ʹ��RTPģ����
			rtpModule->unpackRTP(incomingMsg);

			//4. ȡ������õ�����
			WaitForSingleObject(hsRTPUnpacked, INFINITE);

			rtpModule->getMedia(img.vec);

			//5.�ѽ���õ���������ͼƬ���棨�ź����ڻ����м������ֹͣ��ʱ
			imgBuf->pushBuffer(img.head, img.vec);

			timingModule->endTiming();
		}
		else
		{
			//��������
			cout << "Receive Message:" << incomingMsg << endl;

			//3.������ʲô���͵����������ͷ���ͻ��˵�����ֻ��Ҫ����RTSP�����ͼ��ͷ����
			if (incomingMsg.find("RTSP"))
			{
				//RTSP����

				//4.����RTSPģ�����
				int errCode = 200;

				errCode = rtspModule->decodeMsg(incomingMsg);

				//���OK���ܼ��������԰�
				if (errCode == 200)
				{
					ReleaseSemaphore(hsIsRTSPOK, 1, NULL);
				}
			}
			else
			{
				//ͼ��ͷ����

				//4.ȡ��ͼ��ͷ����
				imgMsgCombine *imgMsg = (imgMsgCombine *)&incomingMsg;

				img.head.imgType = imgMsg->imgHead.imgType;

				img.head.channels = imgMsg->imgHead.imgChannels;

				img.head.xAxis.cols = imgMsg->imgHead.imgCols;

				img.head.yAxis.rows = imgMsg->imgHead.imgRows;
			}
		}		
	}

	return 0;
}

DWORD mwPlayCtrl::mwHeartBeat(LPVOID lparam)
{
	cnctHandler *networkModule = cnctHandler::getInstance();

	rtspHandler *rtspModule = rtspHandler::getInstance();

	string msg;

	WaitForSingleObject(heStartHeartBeat, INFINITE);

	while (1)
	{
		if (WaitForSingleObject(heStartHeartBeat, 0) != WAIT_OBJECT_0)
		{
			break;
		}

		/*
			ÿ30�뷢��һ������
		*/

		Sleep(30000);

		msg = rtspModule->encodeMsg(GET_PARAMETER);

		networkModule->sendMessage(msg);
	}

	return 0;
}

mwPlayCtrl::mwPlayCtrl()
{

}