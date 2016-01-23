#include "testServer.h"

DWORD WINAPI workerThreadFunc(LPVOID lparam);            //线程处理函数

int main()
{
	WSADATA wsaData;
	HANDLE hCompletionPort;
	SYSTEM_INFO sysInfo;       //存储系统信息的结构	
	LPPER_IO_DATA PerIoData;             //单IO数据
	LPPER_HANDLE_DATA PerHandleData;       //单句柄数据

	SOCKADDR_IN srvAddr;
	SOCKET srvSocket;                

	string srvPort = "8554";

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);   //建立完成端口

	GetSystemInfo(&sysInfo);
	int cntThread = sysInfo.dwNumberOfProcessors;
	for (int i = 0; i < cntThread; i++) {
		// 创建服务器工作器线程，并将完成端口传递到该线程
		CreateThread(NULL, NULL, workerThreadFunc, (LPVOID)hCompletionPort, NULL, NULL);
	}

	// 建立流式套接字，由于要使用重叠I/O，这里必须要使用WSASocket来初始化Socket
	srvSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	//获取并填写服务器信息：监听任意地址
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

		//保存客户端信息
		PerHandleData = (LPPER_HANDLE_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_HANDLE_DATA));
		//PerHandleData->clientAddr = clientAddr;
		PerHandleData->clientSocket = acptSocket;
		memcpy(&(PerHandleData->clientAddr), &clientAddr, addrLen);

		//将接受套接字和完成端口关联
		CreateIoCompletionPort((HANDLE)acptSocket, hCompletionPort, (DWORD)PerHandleData, 0);

		//准备一个重叠I/O
		PerIoData = (LPPER_IO_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_IO_DATA));
		ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED));

		PerIoData->wsaBuf.len = BUF_SIZE;              //缓存为最大
		PerIoData->wsaBuf.buf = PerIoData->buffer;     //缓存
		PerIoData->operationType = compRecv;	       //设置模式为接收

		//在新建的套接字上投递一个或多个异步WSARecv或WSASend请求，并立即返回
		WSARecv(PerHandleData->clientSocket, &PerIoData->wsaBuf, 1, &PerIoData->bytesRecv, &PerIoData->flags,
			&(PerIoData->overlapped), NULL);
	}

	return 0;
}

//==========工作者线程===========//
DWORD WINAPI workerThreadFunc(LPVOID lparam)
{
	//处理传入参数
	HANDLE hCompletionPort = (HANDLE)lparam;

	SOCKET clientSocket;

	DWORD bytesTransferred;
	DWORD flags;

	LPPER_HANDLE_DATA handleInfo;
	LPPER_IO_DATA ioInfo;

	string buf;

	while (true)
	{
		//得到完成端口的状态
		//相关数据已经在2-4的参数里面了
		GetQueuedCompletionStatus(hCompletionPort, &bytesTransferred, (LPDWORD)&handleInfo, (LPOVERLAPPED *)&ioInfo, INFINITE);

		clientSocket = handleInfo->clientSocket;

		//首先解析收到的信息
		//首先确认，这个是控制信令，还是流媒体信令
		//TODO：那么我需要设计控制指令了






		//准备下一个，投递一个WSARecv
		//准备一个重叠I/O
		ioInfo = (LPPER_IO_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_IO_DATA));
		ZeroMemory(ioInfo, sizeof(ioInfo));

		ioInfo->wsaBuf.len = BUF_SIZE;             //缓存为最大
		ioInfo->wsaBuf.buf = ioInfo->buffer;       //缓存
		ioInfo->operationType = compRecv;	       //设置模式为接收

		WSARecv(clientSocket, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
	}

	return 0;
}