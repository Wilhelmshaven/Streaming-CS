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

}

cnctHandler::~cnctHandler()
{
	closesocket(srvSocket);
}

//一系列返回信息的函数
cnctHandler* cnctHandler::getInstance()
{
	return instance;
}

//获取系统信息
void cnctHandler::getSystemInfo()
{
	GetSystemInfo(&sysInfo);
}

//建立工作者线程
void cnctHandler::buildThread()
{
	int cntThread = sysInfo.dwNumberOfProcessors * 2;

	workerThread.resize(cntThread);

	for (int i = 0; i < (cntThread); i++) {
		// 创建服务器工作器线程，并将完成端口传递到该线程
		workerThread[i] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)workerThreadFunc, completionPort, NULL, NULL);
		CloseHandle(workerThread[i]);
	}
}

//==========工作者线程===========//
UINT cnctHandler::workerThreadFunc()
{

}