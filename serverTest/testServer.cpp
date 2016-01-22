#include "testServer.h"

int main()
{
	WSADATA wsaData;
	HANDLE hCompletionPort;
	SYSTEM_INFO sysInfo;       //存储系统信息的结构	
	SOCKADDR_IN srvAddr;       //服务器地址结构
	SOCKET srvSocket;                   //唯一的SOCKET，避免乱改

	string srvPort = "80";

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);   //建立完成端口

	//获取并填写服务器信息：监听任意地址
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(stoi(srvPort, nullptr, 10));

	// 建立流式套接字，由于要使用重叠I/O，这里必须要使用WSASocket来初始化Socket
	srvSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	bind(srvSocket, (SOCKADDR*)&srvAddr, sizeof(srvAddr));
	listen(srvSocket, SOMAXCONN);

	while (1)
	{
		SOCKET acptSocket;
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(clientAddr);

		acptSocket = accept(srvSocket, (SOCKADDR*)&clientAddr, &addrLen);


	}

	return 0;
}