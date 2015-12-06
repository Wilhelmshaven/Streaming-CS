#include "CompletionPort.h"

//初始化环境
Comport::Comport()
{
	ServerPort = new char[5];

	//初始化Winsock，版本2.2
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//建立一个完成端口
	CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	//获得本机系统信息
	GetSystemInfo(&SysInfo);

	// 基于处理器的核心数量创建线程池
	HANDLE *WorkerThread = new HANDLE[SysInfo.dwNumberOfProcessors * 2];        //工作者线程句柄
	for (int i = 0; i < (SysInfo.dwNumberOfProcessors * 2); i++){
		// 创建服务器工作器线程，并将完成端口传递到该线程
		WorkerThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerWorkThread, CompletionPort, 0, NULL);
		CloseHandle(WorkerThread[i]);
	}

}

//设置服务器端口的函数
void Comport::SetPort(char *port)
{
	//绑定端口号
	ServerPort = port;
}

//绑定函数
void Comport::BindSocket()
{
	// 建立流式套接字，如果要使用重叠I/O的话，这里必须要使用WSASocket来初始化Socket
	srvSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	// 绑定SOCKET到本机：任意地址
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(atoi(ServerPort));

	bind(srvSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR));
}

//判断连接是否已断开
bool Comport::IsSocketAlive(SOCKET s)
{
	int nByteSent = send(s, "", 0, 0);
	if (nByteSent == -1) return false;
	return true;
}

//监听
void Comport::StartListen()
{
	SOCKADDR_IN saRemote;
	int RemoteLen = sizeof(saRemote);
	PER_IO_OPERATION_DATA *PerIoData;   //单IO数据
	PER_HANDLE_DATA PerHandleData;       //单句柄数据
	SOCKET acptSocket = NULL;            //临时SOCKET
	char *msg = new char[BUF_SIZE];

	// 将SOCKET设置为监听模式
	listen(srvSocket, SOMAXCONN);

	// 开始处理IO数据
	int i = 0;
	while (true){
		// 接收连接，并分配完成端，下次考虑更换成AcceptEx()：accept、WSAAccept是同步操作，AcceptEx是异步操作
		acptSocket = accept(srvSocket, (SOCKADDR*)&saRemote, &RemoteLen);

		PerHandleData.ClientSocket = acptSocket;       //将SOCKET存入结构中
		PerHandleData.ClientAddr = saRemote;
		i++;

		// 将接受套接字和完成端口关联
		CreateIoCompletionPort((HANDLE)acptSocket, CompletionPort, (DWORD)acptSocket, 0);

		// 开始在接受套接字上处理I/O，使用重叠I/O机制
		PerIoData = (PER_IO_OPERATION_DATA *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PER_IO_OPERATION_DATA));

		ZeroMemory(PerIoData, sizeof(PerIoData));
		PerIoData->databuff.len = BUF_SIZE;
		PerIoData->databuff.buf = PerIoData->buffer;
		PerIoData->OperationType = 0;	// RECV_POSTED

		// 在新建的套接字上投递一个或多个异步WSARecv或WSASend请求，这些I/O请求完成后，工作者线程会为I/O请求提供服务单I/O操作数据(I/O重叠)
		WSARecv(PerHandleData.ClientSocket, &PerIoData->databuff, 1, &PerIoData->NumberOfBytesRecv, &PerIoData->Flags, &(PerIoData->overlapped), NULL);

		//更新状态信息
		//if (!IsSocketAlive(acptSocket))
		//{
		//	uWin.UpdateStatusBar(CONNECTION_BREAKS, 0, inet_ntoa(PerHandleData.ClientAddr.sin_addr));
		//	uWin.ModifyList(acptSocket, "Disconnected");
		//}
		//else
		//{
		//	if (PerIoData->NumberOfBytesRecv != 0)
		//	{
		//		//发送成功
		//		uWin.UpdateStatusBar(FILE_REQ, PerIoData->databuff.buf, inet_ntoa(PerHandleData.ClientAddr.sin_addr));
		//	}
		//	else
		//	{
		//		//发送失败
		//		uWin.UpdateStatusBar(FILE_NOT_FOUND, PerIoData->databuff.buf, inet_ntoa(PerHandleData.ClientAddr.sin_addr));
		//	}
		//}
	}
}

//析构函数
Comport::~Comport()
{
	//清理工作
	for (int i = 0; i < SysInfo.dwNumberOfProcessors * 2; i++)
	{
		PostQueuedCompletionStatus(CompletionPort, 0, 0, NULL);
	}

	CloseHandle(CompletionPort);
	shutdown(srvSocket, SD_BOTH);
	closesocket(srvSocket);
	WSACleanup();
}

// 工作线程
DWORD WINAPI ServerWorkThread(LPVOID lpParam)
{
	HANDLE CompletionPort = (HANDLE)lpParam;              //这里实际上是一个ID
	DWORD BytesTransferred;
	SOCKET acptSocket = NULL;
	PER_IO_OPERATION_DATA *PerIoData = NULL;
	LPOVERLAPPED lpOverlapped = NULL;
	char *FileName = new char[MAX_PATH];                       //Windows中最大文件名为260
	char *buf = new char[BUF_SIZE];
	BY_HANDLE_FILE_INFORMATION *FileInfo = new BY_HANDLE_FILE_INFORMATION;

	MsgHeader *msg = new MsgHeader;
	ZeroMemory(msg, sizeof(msg));

	while (true){
		//得到完成端口的状态
		GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&acptSocket, (LPOVERLAPPED *)&PerIoData, INFINITE);

		// 开始数据处理，接收来自客户端的数据
		//sprintf_s(FileName, MAX_PATH, "C:\\%s", PerIoData->databuff.buf);
		//int by = recv(acptSocket, buf, BUF_SIZE, 0);
		//msg = (MsgHeader *)buf;

		//HANDLE hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);
		////查找对应文件名的文件
		//if (hFile != INVALID_HANDLE_VALUE)
		//{
		//	if (msg->type == 1)
		//	{
		//		//获取文件大小等信息
		//		GetFileInformationByHandle(hFile, FileInfo);

		//		//传输文件大小
		//		msg->offsetHigh = FileInfo->nFileSizeHigh;
		//		msg->offsetLow = FileInfo->nFileSizeLow;

		//		send(acptSocket, buf, strlen(buf), 0);
		//	}

		//	if (msg->type == 2)
		//	{
		//		//传输文件请求
		//		//lpOverlapped->Offset = msg->offsetLow;
		//		//lpOverlapped->OffsetHigh = msg->offsetHigh;
		//	}
		//}
		//CloseHandle(hFile);

		// 为下一个重叠调用建立单I/O操作数据
		ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
		PerIoData->databuff.len = BUF_SIZE;
		PerIoData->databuff.buf = PerIoData->buffer;
		PerIoData->OperationType = 0;// RECV_POSTED

		WSARecv(acptSocket,            // 投递这个操作的套接字  
			&(PerIoData->databuff),    // 接收缓冲区，这里需要一个由WSABUF结构构成的数组
			1,                         // 数组中WSABUF结构的数量，设置为1即可
			&PerIoData->NumberOfBytesRecv,                // 如果接收操作立即完成，这里会返回函数调用所接收到的字节数  
			&PerIoData->Flags,                         // 设置为0即可
			&(PerIoData->overlapped),  // 这个Socket对应的重叠结构
			NULL);                     // 这个参数只有完成例程模式才会用到，完成端口中我们设置为NULL即可

	}

	return 0;
}