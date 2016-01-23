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
	srvSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	bind(srvSocket, (SOCKADDR *)&srvAddr, sizeof(srvAddr));
}
cnctHandler::~cnctHandler()
{
	//�˳���ɶ˿�
	for (int i = 0; i < sysInfo.dwNumberOfProcessors; i++)
	{
		PostQueuedCompletionStatus(completionPort, 0, 0, NULL);
	}

	CloseHandle(completionPort);

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
bool cnctHandler::isSocketAlive(SOCKET clientSocket)
{
	string msg = "isAlive?";
	int byteSent;

	byteSent = send(clientSocket, msg.c_str(), msg.size(), NULL);

	if (byteSent == -1)return false;
	else return true;
}

//�����������߳�
int cnctHandler::buildThread()
{
	int cntThread = sysInfo.dwNumberOfProcessors;

	workerThread.resize(cntThread);

	for (int i = 0; i < cntThread; i++) {
		// �����������������̣߳�������ɶ˿ڴ��ݵ����߳�
		workerThread[i] = CreateThread(NULL, NULL, workerThreadFunc, (LPVOID)completionPort, NULL, NULL);

		//CloseHandle(workerThread[i]);
	}

	return 0;
}

//����������
int cnctHandler::startServer()
{						
	//����ɵ���ˣ�ǰ����ô�ຯ������˶�û���á���
	//��Ӧ�ø���������
	getSystemInfo();
	buildThread();

	listen(srvSocket, SOMAXCONN);                //��SOCKET����Ϊ����ģʽ

	//���У��������NEW�����򴴽��̺߳�ṹ�屻ɾ�������޷����롣�߳�����ɾ��
	acptThreadParam *param = new acptThreadParam;
	param->comp = completionPort;
	param->sock = srvSocket;

	CreateThread(NULL, NULL, acptThread, param, NULL, NULL);  //���������߳�

	return 0;
}

//============�����̣߳�ר�����ڽ��մ�������=============//
DWORD WINAPI cnctHandler::acptThread(LPVOID lparam)
{
	//���������
	acptThreadParam *param = (acptThreadParam *)lparam;
	HANDLE hCompletionPort = param->comp;
	SOCKET srvSocket = param->sock;
	delete lparam;

	LPPER_IO_DATA PerIoData;             //��IO����
	LPPER_HANDLE_DATA PerHandleData;       //���������

	SOCKET acptSocket;                   //��ʱSOCKET

	SOCKADDR_IN clientAddr;
	int addrSize = sizeof(clientAddr);

	while (true)
	{
		//if (WaitForSingleObject(hSrvShutdown, 0))break;  //���������û�����Ǹ�����

		//��������
		//TODO�����Ǹ�����AcceptEx()��accept��WSAAccept��ͬ��������AcceptEx���첽����
		acptSocket = accept(srvSocket, (SOCKADDR*)&clientAddr, &addrSize);

		//����ͻ�����Ϣ
		PerHandleData = (LPPER_HANDLE_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_HANDLE_DATA));
		PerHandleData->clientAddr = clientAddr;
		PerHandleData->clientSocket = acptSocket;

		//�������׽��ֺ���ɶ˿ڹ���
		CreateIoCompletionPort((HANDLE)acptSocket, hCompletionPort, (DWORD)PerHandleData, 0);

		//׼��һ���ص�I/O
		PerIoData = (LPPER_IO_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_IO_DATA));	
		ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED));   //����������Ҫ��������ᵼ��GetQueuedCompletionStatus������

		PerIoData->wsaBuf.len = BUF_SIZE;              //����Ϊ���
		PerIoData->wsaBuf.buf = PerIoData->buffer;     //����
		PerIoData->operationType = compRecv;	       //����ģʽΪ����
			
		//���½����׽�����Ͷ��һ�������첽WSARecv��WSASend���󣬲���������
		WSARecv(PerHandleData->clientSocket, &PerIoData->wsaBuf, 1, &PerIoData->bytesRecv, &PerIoData->flags,
			&(PerIoData->overlapped), NULL);
	}

	return 0;
}

//==========�������߳�===========//
DWORD WINAPI cnctHandler::workerThreadFunc(LPVOID lparam)
{	
	//���������
	HANDLE hCompletionPort = (HANDLE)lparam;

	SOCKET clientSocket;

	DWORD bytesTransferred;
	DWORD flags;

	LPPER_HANDLE_DATA handleInfo;
	LPPER_IO_DATA ioInfo;
	
	string buf;

	while (true)
	{
		//�õ���ɶ˿ڵ�״̬
		//��������Ѿ���2-4�Ĳ��������ˣ��ڶ���ָʾ�յ����ֽ������������ǿͻ�����Ϣ�����ĸ�����ؽ�������
		GetQueuedCompletionStatus(hCompletionPort, &bytesTransferred, (LPDWORD)&handleInfo, (LPOVERLAPPED *)&ioInfo, INFINITE);

		//������������
		if (WaitForSingleObject(hSrvShutdown, 0))break;  //���������û�����Ǹ�����

		clientSocket = handleInfo->clientSocket;

		//���Ƚ����յ�����Ϣ
		//����ȷ�ϣ�����ǿ������������ý������
		//TODO����ô����Ҫ��ƿ���ָ����






		//׼����һ����Ͷ��һ��WSARecv
		//׼��һ���ص�I/O
		ioInfo = (LPPER_IO_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_IO_DATA));
		ZeroMemory(ioInfo, sizeof(ioInfo));

		ioInfo->wsaBuf.len = BUF_SIZE;             //����Ϊ���
		ioInfo->wsaBuf.buf = ioInfo->buffer;       //����
		ioInfo->operationType = compRecv;	       //����ģʽΪ����

		WSARecv(clientSocket, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
	}

	return 0;
}