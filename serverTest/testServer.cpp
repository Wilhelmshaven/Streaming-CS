#include "testServer.h"

DWORD WINAPI workerThreadFunc(LPVOID lparam);            //�̴߳�����

int main()
{
	WSADATA wsaData;
	HANDLE hCompletionPort;
	SYSTEM_INFO sysInfo;       //�洢ϵͳ��Ϣ�Ľṹ	
	LPPER_IO_DATA PerIoData;             //��IO����
	LPPER_HANDLE_DATA PerHandleData;       //���������

	SOCKADDR_IN srvAddr;
	SOCKET srvSocket;                

	string srvPort = "8554";

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);   //������ɶ˿�

	GetSystemInfo(&sysInfo);
	int cntThread = sysInfo.dwNumberOfProcessors;
	for (int i = 0; i < cntThread; i++) {
		// �����������������̣߳�������ɶ˿ڴ��ݵ����߳�
		CreateThread(NULL, NULL, workerThreadFunc, (LPVOID)hCompletionPort, NULL, NULL);
	}

	// ������ʽ�׽��֣�����Ҫʹ���ص�I/O���������Ҫʹ��WSASocket����ʼ��Socket
	srvSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	//��ȡ����д��������Ϣ�����������ַ
	ZeroMemory(&srvAddr, sizeof(srvAddr));
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(stoi(srvPort, nullptr, 10));

	bind(srvSocket, (SOCKADDR*)&srvAddr, sizeof(srvAddr));
	listen(srvSocket, SOMAXCONN);

	while (1)
	{
		SOCKET acptSocket;
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(clientAddr);

		acptSocket = accept(srvSocket, (SOCKADDR*)&clientAddr, &addrLen);

		//����ͻ�����Ϣ
		PerHandleData = (LPPER_HANDLE_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_HANDLE_DATA));
		//PerHandleData->clientAddr = clientAddr;
		PerHandleData->clientSocket = acptSocket;
		memcpy(&(PerHandleData->clientAddr), &clientAddr, addrLen);

		//�������׽��ֺ���ɶ˿ڹ���
		CreateIoCompletionPort((HANDLE)acptSocket, hCompletionPort, (DWORD)PerHandleData, 0);

		//׼��һ���ص�I/O
		PerIoData = (LPPER_IO_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_IO_DATA));
		ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED));

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
DWORD WINAPI workerThreadFunc(LPVOID lparam)
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
		//��������Ѿ���2-4�Ĳ���������
		GetQueuedCompletionStatus(hCompletionPort, &bytesTransferred, (LPDWORD)&handleInfo, (LPOVERLAPPED *)&ioInfo, INFINITE);

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