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
#include "crtlMsgHandler.h"

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

	while (1)
	{
		WaitForSingleObject(hsCtrlMsgArrived, INFINITE);

		if (WaitForSingleObject(hSrvShutdown, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		//TODO��������������
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