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

	//��ȡ����д��������Ϣ�����������ַ
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(stoi(srvPort, nullptr, 10));

	// ������ʽ�׽��֣�����Ҫʹ���ص�I/O���������Ҫʹ��WSASocket����ʼ��Socket
	srvSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
}
cnctHandler::~cnctHandler()
{
	//�˳���ɶ˿�
	for (int i = 0; i < sysInfo.dwNumberOfProcessors * 2; i++)
	{
		PostQueuedCompletionStatus(completionPort, 0, 0, NULL);
	}

	CloseHandle(completionPort);
	CloseHandle(acptThread);

	shutdown(srvSocket, SD_BOTH);
	closesocket(srvSocket);

	WSACleanup();
}

//һϵ�з�����Ϣ�ĺ���
cnctHandler* cnctHandler::getInstance()
{
	return instance;
}

//���÷�����
int cnctHandler::srvConfig(string port)
{
	srvPort = port;

	return 0;
}

//��ȡϵͳ��Ϣ
int cnctHandler::getSystemInfo()
{
	GetSystemInfo(&sysInfo);

	return 0;
}

//���ͻ����Ƿ�
bool cnctHandler::isSocketAlive()
{
	string msg = "isAlive?";
	int byteSent;

	byteSent = send(srvSocket, msg.c_str(), msg.size(), NULL);

	if (byteSent == -1)return false;
	else return true;
}

//�����������߳�
int cnctHandler::buildThread()
{
	int cntThread = sysInfo.dwNumberOfProcessors * 2;

	workerThread.resize(cntThread);

	for (int i = 0; i < (cntThread); i++) {
		// �����������������̣߳�������ɶ˿ڴ��ݵ����߳�
		workerThread[i] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)workerThreadFunc, completionPort, NULL, NULL);
		CloseHandle(workerThread[i]);
	}

	return 0;
}

//����������
int cnctHandler::startServer()
{									 
	listen(srvSocket, SOMAXCONN);        //��SOCKET����Ϊ����ģʽ

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)acptThread, NULL, NULL, NULL);  //���������߳�

	return 0;
}

//==========�����߳�=============//
UINT cnctHandler::acptThread()
{
	SOCKADDR_IN clientAddr;
	int addrSize = sizeof(clientAddr);
	PER_IO_OPERATION_DATA *PerIoData;    //��IO����
	PER_HANDLE_DATA PerHandleData;       //���������
	SOCKET acptSocket = NULL;            //��ʱSOCKET

	while (true)
	{
		// �������ӣ���������ɶˣ��´ο��Ǹ�����AcceptEx()��accept��WSAAccept��ͬ��������AcceptEx���첽����
		acptSocket = accept(srvSocket, (SOCKADDR*)&clientAddr, &addrSize);

		//����ͻ�����Ϣ
		PerHandleData.clientAddr = clientAddr;
		PerHandleData.clientSocket = acptSocket;

		// �������׽��ֺ���ɶ˿ڹ���
		CreateIoCompletionPort((HANDLE)acptSocket, completionPort, (DWORD)acptSocket, 0);

		// ��ʼ�ڽ����׽����ϴ���I/O��ʹ���ص�I/O����
		PerIoData = (PER_IO_OPERATION_DATA *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PER_IO_OPERATION_DATA));

		// ���½����׽�����Ͷ��һ�������첽WSARecv��WSASend������ЩI/O������ɺ󣬹������̻߳�ΪI/O�����ṩ����I/O��������(I/O�ص�)
		ZeroMemory(PerIoData, sizeof(PerIoData));
		PerIoData->databuff.len = BUF_SIZE;
		PerIoData->databuff.buf = PerIoData->buffer;
		PerIoData->operationType = 0;	// RECV_POSTED
										
		WSARecv(PerHandleData.clientSocket, &PerIoData->databuff, 1, &PerIoData->bytesRecv, &PerIoData->flags,
			&(PerIoData->overlapped), NULL);
	}
}

//==========�������߳�===========//
UINT cnctHandler::workerThreadFunc()
{
	DWORD BytesTransferred;
	SOCKET acptSocket = NULL;
	PER_IO_OPERATION_DATA *PerIoData = NULL;
	LPOVERLAPPED lpOverlapped = NULL;
	string buf;

	while (true)
	{
		//�õ���ɶ˿ڵ�״̬
		GetQueuedCompletionStatus(completionPort, &BytesTransferred, (PULONG_PTR)&acptSocket, (LPOVERLAPPED *)&PerIoData, INFINITE);

	}

}