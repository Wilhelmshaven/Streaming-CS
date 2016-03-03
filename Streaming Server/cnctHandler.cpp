/*--Author：李宏杰--*/

#include "cnctHandler.h"

//加载消息中间件模块
#include "middleWare.h"

cnctHandler *cnctHandler::instance = new cnctHandler;

queue<string> cnctHandler::rtspQueue, cnctHandler::ctrlQueue;
queue<SOCKET> cnctHandler::socketQueue;

/*
	构造函数

	值：
	开启Winsock
	服务器默认端口为8554
	建立完成端口
	初始化服务器监听地址为任意地址
	初始化Socket
	绑定
*/
cnctHandler::cnctHandler()
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//默认端口
	srvPort = "8554";        

	//建立完成端口
	completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);   

	//获取并填写服务器信息：监听任意地址
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(stoi(srvPort, nullptr, 10));

	// 建立流式套接字，由于要使用重叠I/O，这里必须要使用WSASocket来初始化Socket
	srvSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	bind(srvSocket, (SOCKADDR *)&srvAddr, sizeof(srvAddr));
}

/*
	析构函数
*/
cnctHandler::~cnctHandler()
{
	delete param;

	//退出完成端口
	for (int i = 0; i < sysInfo.dwNumberOfProcessors; i++)
	{
		PostQueuedCompletionStatus(completionPort, 0, 0, NULL);
	}

	CloseHandle(completionPort);

	shutdown(srvSocket, SD_BOTH);
	closesocket(srvSocket);

	WSACleanup();
}

cnctHandler* cnctHandler::getInstance()
{
	return instance;
}

/*
	设置服务器：输入端口号，返回值为0

	TODO：返回值为错误码
*/
int cnctHandler::srvConfig(int port)
{
	srvPort = to_string(port);

	return 0;
}

/*
	检查客户端是否活动，输入Socket，返回布尔值
*/
bool cnctHandler::isSocketAlive(SOCKET clientSocket)
{
	string msg = "isAlive?";
	int byteSent;

	byteSent = send(clientSocket, msg.c_str(), msg.size(), NULL);

	if (byteSent == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/*
	函数：建立工作者线程
*/
void cnctHandler::buildThread()
{
	//获取系统信息（最主要是CPU核数，以便创建Worker线程）
	GetSystemInfo(&sysInfo);

	int cntThread = sysInfo.dwNumberOfProcessors;

	workerThread.resize(cntThread);

	for (int i = 0; i < cntThread; i++) {

		// 创建服务器工作器线程，并将完成端口传递到该线程
		workerThread[i] = CreateThread(NULL, NULL, workerThreadFunc, (LPVOID)completionPort, NULL, NULL);

		//CloseHandle(workerThread[i]);
	}
}

/*
	函数：启动服务器
*/
int cnctHandler::startServer()
{	
	int status = 0;

	//真是傻逼了，前面那么多函数设计了都没调用……
	//真应该搞个防呆设计
	//以及，封装过度也不是什么好事情

	buildThread();

	//将SOCKET设置为监听模式
	status = listen(srvSocket, SOMAXCONN);                

	//徐行指导说：这里必须NEW，否则创建线程后结构体被删除，就无法传入。线程中再删除（如果不再使用）
	param = new acptThreadParam;
	param->comp = completionPort;
	param->sock = srvSocket;

	//创建接收线程
	CreateThread(NULL, NULL, acptThread, param, NULL, NULL);  

	return status;
}

string cnctHandler::getRTSPMsg()
{
	string msg;

	if (!rtspQueue.empty())
	{
		msg = rtspQueue.front();
		rtspQueue.pop();
	}

	return msg;
}

string cnctHandler::getCtrlMsg()
{
	string msg;

	if (!ctrlQueue.empty())
	{
		msg = ctrlQueue.front();
		ctrlQueue.pop();
	}

	return msg;
}

SOCKET cnctHandler::getSocket()
{
	SOCKET soc;

	if (!socketQueue.empty())
	{
		soc = socketQueue.front();

		socketQueue.pop();
	}

	return soc;
}

int cnctHandler::sendMessage(SOCKET socket, string msg)
{
	int bytesSent;

	bytesSent = send(socket, msg.c_str(), msg.length(), NULL);

	return bytesSent;
}

/*
	监听线程，专门用于接收传入连接
*/
DWORD WINAPI cnctHandler::acptThread(LPVOID lparam)
{
	//处理传入参数
	acptThreadParam *param = (acptThreadParam *)lparam;
	HANDLE hCompletionPort = param->comp;
	SOCKET srvSocket = param->sock;

	//单IO数据
	LPPER_IO_DATA PerIoData;         

	//单句柄数据
	LPPER_HANDLE_DATA PerHandleData;     

	//临时SOCKET
	SOCKET acptSocket;    

	SOCKADDR_IN clientAddr;
	int addrSize = sizeof(clientAddr);

	while (true)
	{
		//if (WaitForSingleObject(hSrvShutdown, 0))break;  //这个到底有没有用是个问题

		/*
			接收连接
			TODO：考虑更换成AcceptEx()：accept、WSAAccept是同步操作，AcceptEx是异步操作
		*/
		acptSocket = accept(srvSocket, (SOCKADDR*)&clientAddr, &addrSize);

		//保存客户端信息
		PerHandleData = (LPPER_HANDLE_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_HANDLE_DATA));
		PerHandleData->clientAddr = clientAddr;
		PerHandleData->clientSocket = acptSocket;

		//将接受套接字和完成端口关联
		CreateIoCompletionPort((HANDLE)acptSocket, hCompletionPort, (DWORD)PerHandleData, 0);

		/*
			准备一个重叠I/O
		*/
		PerIoData = (LPPER_IO_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_IO_DATA));	

		//这个清零很重要！！否则会导致GetQueuedCompletionStatus不返回
		ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED));   

		//缓存为最大
		PerIoData->wsaBuf.len = BUF_SIZE;         

		PerIoData->wsaBuf.buf = PerIoData->buffer; 

		//设置模式为接收
		PerIoData->operationType = compRecv;	       
			
		//在新建的套接字上投递一个或多个异步WSARecv或WSASend请求，并立即返回
		WSARecv(PerHandleData->clientSocket, &PerIoData->wsaBuf, 1, &PerIoData->bytesRecv, &PerIoData->flags,
			&(PerIoData->overlapped), NULL);
	}

	delete param;

	return 0;
}

/*
	工作者线程
*/
DWORD WINAPI cnctHandler::workerThreadFunc(LPVOID lparam)
{	
	mwMsg *mwMsgHandler = mwMsg::getInstance();

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
		/*
			得到完成端口的状态
			相关数据已经在2-4的参数里面了：第二个指示收到的字节数，第三个是客户端信息，第四个是相关接收数据
		*/
		GetQueuedCompletionStatus(hCompletionPort, &bytesTransferred, (LPDWORD)&handleInfo, (LPOVERLAPPED *)&ioInfo, INFINITE);

		//结束服务器？
		if (WaitForSingleObject(hSrvShutdown, 0))break;  //这个到底有没有用是个问题

		clientSocket = handleInfo->clientSocket;

		/*
			处理信令
			首先解析是流媒体还是控制，然后塞到对应队列中，最后激活信号量
			注意把SOCKET一起丢进去……要不然回发，回给谁啊？？？！
		*/

		buf = ioInfo->buffer;

		if (buf.find("RTSP"))
		{
			rtspQueue.push(buf);

			socketQueue.push(handleInfo->clientSocket);

			ReleaseSemaphore(hsRTSPMsgArrived, 1, NULL);
		}
		else
		{
			ctrlQueue.push(buf);

			socketQueue.push(handleInfo->clientSocket);

			ReleaseSemaphore(hsCtrlMsgArrived, 1, NULL);
		}

		/*
			准备下一个连接，投递一个WSARecv
		*/

		//准备一个重叠I/O
		ioInfo = (LPPER_IO_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPPER_IO_DATA));
		ZeroMemory(ioInfo, sizeof(ioInfo));

		//缓存为最大
		ioInfo->wsaBuf.len = BUF_SIZE;    

		ioInfo->wsaBuf.buf = ioInfo->buffer; 

		//设置模式为接收
		ioInfo->operationType = compRecv;	       

		WSARecv(clientSocket, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
	}

	return 0;
}