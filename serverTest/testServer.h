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

//RTSP����
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

	//�������
};
enum CustomDefine
{
	BUF_SIZE = 8192,
	//��������ɶ˿ڵĲ���ģʽ���
	compRecv = 0,
	compSend = 1,
};


//�ص�I/O��Ҫ�õ����࣬��ʱ��¼IO���ݣ������û��ͳһ��׼
typedef struct
{
	WSAOVERLAPPED overlapped;  // ÿһ���ص�I/O���������Ҫ��һ��

	WSABUF wsaBuf;             // 
	char buffer[BUF_SIZE];     // ��ӦWSABUF��Ļ�����
	DWORD bytesRecv;           // ���յ���������
	DWORD flags;               //
	int operationType;         // ��־����ص�I/O��������ʲô�ģ�����Accept/Recv��
} PER_IO_DATA, *LPPER_IO_DATA;

/**
* �ṹ�����ƣ�PER_HANDLE_DATA
* �ṹ��洢����¼�����׽��ֵ����ݣ��������׽��ֵı������׽��ֵĶ�Ӧ�Ŀͻ��˵ĵ�ַ��
* �ṹ�����ã��������������Ͽͻ���ʱ����Ϣ�洢���ýṹ���У�֪���ͻ��˵ĵ�ַ�Ա��ڻطá�
**/
typedef struct
{
	SOCKET clientSocket;       // ���I/O������ʹ�õ�Socket
	SOCKADDR_IN clientAddr;    // ...��ַ��Ϣ
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

DWORD WINAPI thread(LPVOID param);