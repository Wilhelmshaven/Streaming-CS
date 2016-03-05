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

mwPlayCtrl* mwPlayCtrl::instance = new mwPlayCtrl;

mwPlayCtrl * mwPlayCtrl::getInstance()
{
	return instance;
}

void mwPlayCtrl::startMiddleWare()
{
	CreateThread(NULL, NULL, mwCtrlMsgThread, NULL, NULL, NULL);

	CreateThread(NULL, NULL, mwMediaThread, NULL, NULL, NULL);
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

	while (1)
	{
		image img;

		//TODO 1.�ȴ�����ģ�飨�����������յ�RTP����ȡ�ذ���ͼ��ͷ
		string packet;
		
		//TODO 2.ʹ��RTPģ����
		rtpModule->unpackRTP(packet);

		//3. ȡ������õ�����
		WaitForSingleObject(hsRTPUnpacked, INFINITE);

		rtpModule->getMedia(img.vec);

		//4.�ѽ���õ���������ͼƬ���棨�ź����ڻ����м������ֹͣ��ʱ
		imgBuf->pushBuffer(img.head, img.vec);

		timingModule->endTiming();
	}

	return 0;
}

mwPlayCtrl::mwPlayCtrl()
{

}