#pragma once

//I/O Headers
#include <iostream>
#include <fstream>
#include <stdio.h>

// STL Headers
#include <string>

// Winsock Headers
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//
#include <stdlib.h>

using namespace std;

//RTSP信令
enum RTSPmessage
{
	OPTION = 1,
	DESCRIBE = 2,
	SETUP = 3,
	PLAY = 4,
	GET_PARAMETER = 5,
	PAUSE = 6,
	STOP = 7,
	TEARDOWN = 8,

	//错误代码
};
enum CustomDefine
{
	BUF_SIZE = 8192,
	//下面是完成端口的操作模式标记
	compRecv = 0,
	compSend = 1,
};


//重叠I/O需要用到的类，临时记录IO数据，这个并没有统一标准
typedef struct
{
	WSAOVERLAPPED overlapped;  // 每一个重叠I/O网络操作都要有一个

	WSABUF wsaBuf;             // 
	char buffer[BUF_SIZE];     // 对应WSABUF里的缓冲区
	DWORD bytesRecv;           // 接收到的数据量
	DWORD flags;               //
	int operationType;         // 标志这个重叠I/O操作是做什么的，例如Accept/Recv等
} PER_IO_DATA, *LPPER_IO_DATA;

/**
* 结构体名称：PER_HANDLE_DATA
* 结构体存储：记录单个套接字的数据，包括了套接字的变量及套接字的对应的客户端的地址。
* 结构体作用：当服务器连接上客户端时，信息存储到该结构体中，知道客户端的地址以便于回访。
**/
typedef struct
{
	SOCKET clientSocket;       // 这个I/O操作所使用的Socket
	SOCKADDR_IN clientAddr;    // ...地址信息
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

DWORD WINAPI thread(LPVOID param);