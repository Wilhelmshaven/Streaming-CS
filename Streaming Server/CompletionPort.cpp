#include "CompletionPort.h"

//��ʼ������
Comport::Comport()
{
	ServerPort = new char[5];

	//��ʼ��Winsock���汾2.2
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//����һ����ɶ˿�
	CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	//��ñ���ϵͳ��Ϣ
	GetSystemInfo(&SysInfo);

	// ���ڴ������ĺ������������̳߳�
	HANDLE *WorkerThread = new HANDLE[SysInfo.dwNumberOfProcessors * 2];        //�������߳̾��
	for (int i = 0; i < (SysInfo.dwNumberOfProcessors * 2); i++){
		// �����������������̣߳�������ɶ˿ڴ��ݵ����߳�
		WorkerThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerWorkThread, CompletionPort, 0, NULL);
		CloseHandle(WorkerThread[i]);
	}

}

//���÷������˿ڵĺ���
void Comport::SetPort(char *port)
{
	//�󶨶˿ں�
	ServerPort = port;
}

//�󶨺���
void Comport::BindSocket()
{
	// ������ʽ�׽��֣����Ҫʹ���ص�I/O�Ļ����������Ҫʹ��WSASocket����ʼ��Socket
	srvSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	// ��SOCKET�������������ַ
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(atoi(ServerPort));

	bind(srvSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR));
}

//�ж������Ƿ��ѶϿ�
bool Comport::IsSocketAlive(SOCKET s)
{
	int nByteSent = send(s, "", 0, 0);
	if (nByteSent == -1) return false;
	return true;
}

//����
void Comport::StartListen()
{
	SOCKADDR_IN saRemote;
	int RemoteLen = sizeof(saRemote);
	PER_IO_OPERATION_DATA *PerIoData;   //��IO����
	PER_HANDLE_DATA PerHandleData;       //���������
	SOCKET acptSocket = NULL;            //��ʱSOCKET
	char *msg = new char[BUF_SIZE];

	// ��SOCKET����Ϊ����ģʽ
	listen(srvSocket, SOMAXCONN);

	// ��ʼ����IO����
	int i = 0;
	while (true){
		// �������ӣ���������ɶˣ��´ο��Ǹ�����AcceptEx()��accept��WSAAccept��ͬ��������AcceptEx���첽����
		acptSocket = accept(srvSocket, (SOCKADDR*)&saRemote, &RemoteLen);

		PerHandleData.ClientSocket = acptSocket;       //��SOCKET����ṹ��
		PerHandleData.ClientAddr = saRemote;
		i++;

		// �������׽��ֺ���ɶ˿ڹ���
		CreateIoCompletionPort((HANDLE)acptSocket, CompletionPort, (DWORD)acptSocket, 0);

		// ��ʼ�ڽ����׽����ϴ���I/O��ʹ���ص�I/O����
		PerIoData = (PER_IO_OPERATION_DATA *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PER_IO_OPERATION_DATA));

		ZeroMemory(PerIoData, sizeof(PerIoData));
		PerIoData->databuff.len = BUF_SIZE;
		PerIoData->databuff.buf = PerIoData->buffer;
		PerIoData->OperationType = 0;	// RECV_POSTED

		// ���½����׽�����Ͷ��һ�������첽WSARecv��WSASend������ЩI/O������ɺ󣬹������̻߳�ΪI/O�����ṩ����I/O��������(I/O�ص�)
		WSARecv(PerHandleData.ClientSocket, &PerIoData->databuff, 1, &PerIoData->NumberOfBytesRecv, &PerIoData->Flags, &(PerIoData->overlapped), NULL);

		//����״̬��Ϣ
		//if (!IsSocketAlive(acptSocket))
		//{
		//	uWin.UpdateStatusBar(CONNECTION_BREAKS, 0, inet_ntoa(PerHandleData.ClientAddr.sin_addr));
		//	uWin.ModifyList(acptSocket, "Disconnected");
		//}
		//else
		//{
		//	if (PerIoData->NumberOfBytesRecv != 0)
		//	{
		//		//���ͳɹ�
		//		uWin.UpdateStatusBar(FILE_REQ, PerIoData->databuff.buf, inet_ntoa(PerHandleData.ClientAddr.sin_addr));
		//	}
		//	else
		//	{
		//		//����ʧ��
		//		uWin.UpdateStatusBar(FILE_NOT_FOUND, PerIoData->databuff.buf, inet_ntoa(PerHandleData.ClientAddr.sin_addr));
		//	}
		//}
	}
}

//��������
Comport::~Comport()
{
	//������
	for (int i = 0; i < SysInfo.dwNumberOfProcessors * 2; i++)
	{
		PostQueuedCompletionStatus(CompletionPort, 0, 0, NULL);
	}

	CloseHandle(CompletionPort);
	shutdown(srvSocket, SD_BOTH);
	closesocket(srvSocket);
	WSACleanup();
}

// �����߳�
DWORD WINAPI ServerWorkThread(LPVOID lpParam)
{
	HANDLE CompletionPort = (HANDLE)lpParam;              //����ʵ������һ��ID
	DWORD BytesTransferred;
	SOCKET acptSocket = NULL;
	PER_IO_OPERATION_DATA *PerIoData = NULL;
	LPOVERLAPPED lpOverlapped = NULL;
	char *FileName = new char[MAX_PATH];                       //Windows������ļ���Ϊ260
	char *buf = new char[BUF_SIZE];
	BY_HANDLE_FILE_INFORMATION *FileInfo = new BY_HANDLE_FILE_INFORMATION;

	MsgHeader *msg = new MsgHeader;
	ZeroMemory(msg, sizeof(msg));

	while (true){
		//�õ���ɶ˿ڵ�״̬
		GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&acptSocket, (LPOVERLAPPED *)&PerIoData, INFINITE);

		// ��ʼ���ݴ����������Կͻ��˵�����
		//sprintf_s(FileName, MAX_PATH, "C:\\%s", PerIoData->databuff.buf);
		//int by = recv(acptSocket, buf, BUF_SIZE, 0);
		//msg = (MsgHeader *)buf;

		//HANDLE hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);
		////���Ҷ�Ӧ�ļ������ļ�
		//if (hFile != INVALID_HANDLE_VALUE)
		//{
		//	if (msg->type == 1)
		//	{
		//		//��ȡ�ļ���С����Ϣ
		//		GetFileInformationByHandle(hFile, FileInfo);

		//		//�����ļ���С
		//		msg->offsetHigh = FileInfo->nFileSizeHigh;
		//		msg->offsetLow = FileInfo->nFileSizeLow;

		//		send(acptSocket, buf, strlen(buf), 0);
		//	}

		//	if (msg->type == 2)
		//	{
		//		//�����ļ�����
		//		//lpOverlapped->Offset = msg->offsetLow;
		//		//lpOverlapped->OffsetHigh = msg->offsetHigh;
		//	}
		//}
		//CloseHandle(hFile);

		// Ϊ��һ���ص����ý�����I/O��������
		ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
		PerIoData->databuff.len = BUF_SIZE;
		PerIoData->databuff.buf = PerIoData->buffer;
		PerIoData->OperationType = 0;// RECV_POSTED

		WSARecv(acptSocket,            // Ͷ������������׽���  
			&(PerIoData->databuff),    // ���ջ�������������Ҫһ����WSABUF�ṹ���ɵ�����
			1,                         // ������WSABUF�ṹ������������Ϊ1����
			&PerIoData->NumberOfBytesRecv,                // ������ղ���������ɣ�����᷵�غ������������յ����ֽ���  
			&PerIoData->Flags,                         // ����Ϊ0����
			&(PerIoData->overlapped),  // ���Socket��Ӧ���ص��ṹ
			NULL);                     // �������ֻ���������ģʽ�Ż��õ�����ɶ˿�����������ΪNULL����

	}

	return 0;
}