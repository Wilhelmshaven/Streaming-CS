#include "head.h"

int main()
{
	//WSADATA wsaData;
	//WSAStartup(MAKEWORD(2, 2), &wsaData);

	//addrinfo hints;
	//struct addrinfo *res, *cur;
	//int ret;
	//sockaddr_in *addr;
	//char ipbuf[16];

	//ZeroMemory(&hints, sizeof(hints));
	//hints.ai_family = AF_INET; /* Allow IPv4 */
	//hints.ai_flags = AI_PASSIVE; /* For wildcard IP address */
	//hints.ai_protocol = 0; /* Any protocol */
	//hints.ai_socktype = SOCK_STREAM;

	//ret = getaddrinfo("www.163.com", NULL, NULL, &res);

	//for (cur = res; cur != NULL; cur = cur->ai_next) {
	//	addr = (struct sockaddr_in *)cur->ai_addr;
	//	printf("%s\n", inet_ntop(AF_INET,
	//		&addr->sin_addr, ipbuf, 16));
	//}

	HANDLE heartBeatTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	LARGE_INTEGER timeCnt;
	timeCnt.QuadPart = 0;  //6s

	SetWaitableTimer(heartBeatTimer, &timeCnt, 1000, NULL, NULL, 0);

	while (1)
	{
		//检查是否需要结束
		//WaitForSingleObject(handle, 0);	

		
		WaitForSingleObject(heartBeatTimer, INFINITE);                     //等待10秒
		cout << "test." << endl;
	}

	return 0;
}