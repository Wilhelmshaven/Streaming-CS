#include "testServer.h"

int main()
{
	WSADATA wsaData;
	HANDLE hCompletionPort;
	SYSTEM_INFO sysInfo;       //�洢ϵͳ��Ϣ�Ľṹ	
	SOCKADDR_IN srvAddr;       //��������ַ�ṹ
	SOCKET srvSocket;                   //Ψһ��SOCKET�������Ҹ�

	string srvPort = "80";

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);   //������ɶ˿�

	//��ȡ����д��������Ϣ�����������ַ
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(stoi(srvPort, nullptr, 10));

	// ������ʽ�׽��֣�����Ҫʹ���ص�I/O���������Ҫʹ��WSASocket����ʼ��Socket
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