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

	while (1)
	{
		//1.ȡ�߿��ư���
		WaitForSingleObject(hsNewCtrlKey, INFINITE);

		char key = 0;
		key = playerModule->getCtrlKey();

		//2.�ѿ��ư����������������ģ�����֮����ȡ�ر�����
		string msg;
		msg = ctrlMsgModule->keyboardMsgEncode(key);

		//3.�ѱ���õ����������ģ�鷢��֮
		int bytesSent = 0;
		bytesSent = networkModule->sendMessage(msg);
	}

	return 0;
}

DWORD mwPlayCtrl::mwMediaThread(LPVOID lparam)
{
	imgBuffer *imgBuf = imgBuffer::getInstance();

	while (1)
	{
		//TODO 1.�ȴ�RTPģ����յ����ݲ����

		//TODO 2.����������ͼƬ���棨�ź����ڻ����м��
	}

	return 0;
}

mwPlayCtrl::mwPlayCtrl()
{

}