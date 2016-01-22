#include "head.h"

#define BUF_SIZE 8196

int main()
{
	//��ʼ��Winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//����Socket;
	SOCKET rtspSocket = socket(AF_INET, SOCK_STREAM, 0);

	in_addr address;
	sockaddr_in ServerAddr;

	ServerAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &address);
	ServerAddr.sin_addr = address;
	ServerAddr.sin_port = htons(atoi("554"));

	int flag = connect(rtspSocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr));//�����ǿ��ܳ�ʱ�ģ�Ĭ�ϳ�ʱʱ����75��

	if (flag == 0)
	{
		string msg, input;

		while (1)
		{
			cin >> input;
			if (input != "end")
			{
				msg += input;
			}
			else if (input != "break")
			{
				send(rtspSocket, msg.c_str(), msg.length(), 0);
				msg.clear();
			}
			else break;
		}
	}

	return 0;
}