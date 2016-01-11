#include "cnctHandler.h"

//ֱ�ӳ�ʼ��
cnctHandler *cnctHandler::instance = new cnctHandler;

//���������
cnctHandler::cnctHandler()
{
	//VAR init
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	srvPort = "8554";         //Ĭ�϶˿�

	completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);   //������ɶ˿�

}

cnctHandler::~cnctHandler()
{
	closesocket(srvSocket);
}

//һϵ�з�����Ϣ�ĺ���
cnctHandler* cnctHandler::getInstance()
{
	return instance;
}

//��ȡϵͳ��Ϣ
void cnctHandler::getSystemInfo()
{
	GetSystemInfo(&sysInfo);
}

//�����������߳�
void cnctHandler::buildThread()
{
	int cntThread = sysInfo.dwNumberOfProcessors * 2;

	workerThread.resize(cntThread);

	for (int i = 0; i < (cntThread); i++) {
		// �����������������̣߳�������ɶ˿ڴ��ݵ����߳�
		workerThread[i] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)workerThreadFunc, completionPort, NULL, NULL);
		CloseHandle(workerThread[i]);
	}
}

//==========�������߳�===========//
UINT cnctHandler::workerThreadFunc()
{

}