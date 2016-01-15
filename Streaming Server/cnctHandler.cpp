#include "cnctHandler.h"

//直接初始化
cnctHandler *cnctHandler::instance = new cnctHandler;

//构造和析构
cnctHandler::cnctHandler()
{
	//VAR init
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	srvPort = "8554";         //默认端口

	completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);   //建立完成端口

	//获取并填写服务器信息：监听任意地址
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(stoi(srvPort, nullptr, 10));

	// 建立流式套接字，由于要使用重叠I/O，这里必须要使用WSASocket来初始化Socket
	srvSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
}
cnctHandler::~cnctHandler()
{
	//退出完成端口
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

//一系列返回信息的函数
cnctHandler* cnctHandler::getInstance()
{
	return instance;
}

//设置服务器
int cnctHandler::srvConfig(string port)
{
	srvPort = port;

	return 0;
}

//获取系统信息
int cnctHandler::getSystemInfo()
{
	GetSystemInfo(&sysInfo);

	return 0;
}

//检查客户端是否活动
bool cnctHandler::isSocketAlive()
{
	string msg = "isAlive?";
	int byteSent;

	byteSent = send(srvSocket, msg.c_str(), msg.size(), NULL);

	if (byteSent == -1)return false;
	else return true;
}

//建立工作者线程
int cnctHandler::buildThread()
{
	int cntThread = sysInfo.dwNumberOfProcessors * 2;

	workerThread.resize(cntThread);

	for (int i = 0; i < (cntThread); i++) {
		// 创建服务器工作器线程，并将完成端口传递到该线程
		workerThread[i] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)workerThreadFunc, completionPort, NULL, NULL);
		CloseHandle(workerThread[i]);
	}

	return 0;
}

//启动服务器
int cnctHandler::startServer()
{									 
	listen(srvSocket, SOMAXCONN);        //将SOCKET设置为监听模式

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)acptThread, NULL, NULL, NULL);  //创建接收线程

	return 0;
}

//==========监听线程=============//
UINT cnctHandler::acptThread()
{
	SOCKADDR_IN clientAddr;
	int addrSize = sizeof(clientAddr);
	PER_IO_OPERATION_DATA *PerIoData;    //单IO数据
	PER_HANDLE_DATA PerHandleData;       //单句柄数据
	SOCKET acptSocket = NULL;            //临时SOCKET

	while (true)
	{
		// 接收连接，并分配完成端，下次考虑更换成AcceptEx()：accept、WSAAccept是同步操作，AcceptEx是异步操作
		acptSocket = accept(srvSocket, (SOCKADDR*)&clientAddr, &addrSize);

		//保存客户端信息
		PerHandleData.clientAddr = clientAddr;
		PerHandleData.clientSocket = acptSocket;

		// 将接受套接字和完成端口关联
		CreateIoCompletionPort((HANDLE)acptSocket, completionPort, (DWORD)acptSocket, 0);

		// 开始在接受套接字上处理I/O，使用重叠I/O机制
		PerIoData = (PER_IO_OPERATION_DATA *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PER_IO_OPERATION_DATA));

		// 在新建的套接字上投递一个或多个异步WSARecv或WSASend请求，这些I/O请求完成后，工作者线程会为I/O请求提供服务单I/O操作数据(I/O重叠)
		ZeroMemory(PerIoData, sizeof(PerIoData));
		PerIoData->databuff.len = BUF_SIZE;
		PerIoData->databuff.buf = PerIoData->buffer;
		PerIoData->operationType = 0;	// RECV_POSTED
										
		WSARecv(PerHandleData.clientSocket, &PerIoData->databuff, 1, &PerIoData->bytesRecv, &PerIoData->flags,
			&(PerIoData->overlapped), NULL);
	}
}

//==========工作者线程===========//
UINT cnctHandler::workerThreadFunc()
{
	DWORD BytesTransferred;
	SOCKET acptSocket = NULL;
	PER_IO_OPERATION_DATA *PerIoData = NULL;
	LPOVERLAPPED lpOverlapped = NULL;
	string buf;

	while (true)
	{
		//得到完成端口的状态
		GetQueuedCompletionStatus(completionPort, &BytesTransferred, (PULONG_PTR)&acptSocket, (LPOVERLAPPED *)&PerIoData, INFINITE);

	}

}