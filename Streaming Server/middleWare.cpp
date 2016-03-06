/*--Author������--*/

#include "middleWare.h"

/*
	���ظ��м������Ҫ�Խӵ�ģ��
*/

//�������紦��ģ��
#include "cnctHandler.h"

//������ý�������ģ��
#include "rtspHandler.h"

//���ؿ��������ģ��
#include "ctrlMsgHandler.h"

//��������ͷ��ģ����Ⱦ����ģ��
#include "camCap.h"

//����ͼ�񻺴�
#include "imageQueue.h"

//����RTP���ģ��
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

		//1.������ģ���������Լ�SOCKET��
		ctrlMsg = netModule->getCtrlMsg();

		clientSocket = netModule->getSocket();

		//2.�������������ģ�飬����ȡ�Ự��
		session = ctrlMsgModule->decodeMsg(ctrlMsg);

		//3.�Ƚ�����ˣ��û���
		key = 0;

		WaitForSingleObject(hsCtrlMsg, INFINITE);

		key = ctrlMsgModule->getCtrlKey();

		//4.Ȼ��������Ⱦ��
		renderer->render(key);

		//5.Ȼ���ͼ����ˣ�ȡ������ע�⣬��ͼ�񻺴���ȡ
		WaitForSingleObject(hsImageReady, INFINITE);

		imgBuf->popBuffer(imageHead, img);

		//6.��ͼ������RTP���ģ�飬��������SOCKET�Թ������к�
		rtpPacket = rtpModule->pack(clientSocket, img);

		//7.����ͼ��ͷ
		imageHeadMsg = ctrlMsgModule->encodeMsg(imageHead, img.size(), session);

		//8.��RTP����������ģ�鷢�͡��ȷ���ͷ�ٷ�������
		netModule->sendMessage(clientSocket, imageHeadMsg);

		netModule->sendMessage(clientSocket, rtpPacket);
	}

	return 0;
}

/*
	ר�Ŵ�����ý��������߳�
	�Խ�����ģ������ý�������ģ��
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

		//1.������ģ���ȡ����
		rtspMsg = netModule->getRTSPMsg();

		//2.������ģ���ȡSocket
		clientSocket = netModule->getSocket();

		//3.������ý�������ģ����벢��ô���
		rtspMsg = rtspModule->msgCodec(clientSocket, rtspMsg);

		//4.��������ģ�鷢�ʹ���
		bytesSent = netModule->sendMessage(clientSocket, rtspMsg);
	}

	return 0;
}

mwMsg::mwMsg()
{

}